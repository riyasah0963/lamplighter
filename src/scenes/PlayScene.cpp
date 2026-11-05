#include "scenes/PlayScene.h"

#include <cmath>
#include <format>
#include <memory>

#include "ConfigLoader.h"
#include "Creeper.h"
#include "DrawHelpers.h"
#include "GameConstants.h"
#include "GamePaths.h"
#include "LevelLoader.h"
#include "Moth.h"
#include "SceneManager.h"
#include "scenes/ErrorScene.h"
#include "scenes/PauseScene.h"
#include "scenes/ResultScene.h"

namespace
{
    constexpr SDL_Color kLanternColour{ 255, 214, 150, 255 };  // warm oil flame
    constexpr SDL_Color kLampColour{ 255, 196, 120, 255 };     // slightly deeper gold for street lamps

    float distanceBetween(SDL_FPoint a, SDL_FPoint b)
    {
        return std::hypot(a.x - b.x, a.y - b.y);
    }
}

PlayScene::PlayScene(SceneManager& scenes, int district)
    : Scene(scenes)
    , district_(district)
    , balance_(ConfigLoader::load(gamePath("config/balance.txt")))
    , level_(LevelLoader::load(gamePath(std::format("levels/district{}.txt", district))))
    , player_(level_, input_, balance_, level_.objects().playerStart)
    , lighting_(balance_.moonlight)
    , clockSecondsLeft_(balance_.clockSeconds)
{
    for (const TilePos& tile : level_.objects().lamps)
    {
        lamps_.emplace_back(tile);
    }
    for (const TilePos& tile : level_.objects().oilCans)
    {
        oilCans_.emplace_back(tile);
    }
    for (const TilePos& tile : level_.objects().creeperSpawns)
    {
        enemies_.push_back(std::make_unique<Creeper>(level_, balance_, tile));
    }
    for (const TilePos& tile : level_.objects().mothSpawns)
    {
        enemies_.push_back(std::make_unique<Moth>(level_, balance_, tile));
    }
}

std::unique_ptr<Scene> PlayScene::start(SceneManager& scenes, int district)
{
    try
    {
        return std::make_unique<PlayScene>(scenes, district);
    }
    catch (const LevelLoadError& error)
    {
        return std::make_unique<ErrorScene>(scenes, error.what());
    }
    catch (const ConfigLoadError& error)
    {
        return std::make_unique<ErrorScene>(scenes, error.what());
    }
}

void PlayScene::handleEvent(const SDL_Event& event)
{
    if (isKeyPress(event, SDLK_ESCAPE) || isKeyPress(event, SDLK_P))
    {
        scenes().requestPush(std::make_unique<PauseScene>(scenes()));  // Play stays underneath, frozen
    }
    else if (const Command* command = input_.commandFor(event))
    {
        command->execute(*this);  // E lights a lamp, Space opens or shuts the lantern
    }
}

void PlayScene::update(double dt)
{
    elapsedSeconds_ += dt;
    clockSecondsLeft_ -= dt;
    player_.update(dt);
    collectOilCans();
    refillFromLitLamps(dt);

    for (const std::unique_ptr<Enemy>& enemy : enemies_)
    {
        enemy->setSenses(sensesFor(*enemy));
        enemy->update(dt);
    }

    checkEnemyHits();

    // Win: every lamp in the district is lit. Lose: out of hearts, or the candle-clock burned out.
    if (litLampCount() == static_cast<int>(lamps_.size()))
    {
        scenes().requestSwitchTo(std::make_unique<ResultScene>(scenes(), district_, true, elapsedSeconds_));
    }
    else if (player_.hearts() == 0 || clockSecondsLeft_ <= 0.0)
    {
        scenes().requestSwitchTo(std::make_unique<ResultScene>(scenes(), district_, false, elapsedSeconds_));
    }
}

void PlayScene::checkEnemyHits()
{
    // A Creeper's hit only counts in darkness: an open lantern or a lit lamp protects the player.
    const bool playerInLight = player_.lanternOpen() || litLampCovering(player_.centre()) != nullptr;

    const SDL_FRect playerBox = player_.bounds();
    for (const std::unique_ptr<Enemy>& enemy : enemies_)
    {
        const SDL_FRect enemyBox = enemy->bounds();
        if (!SDL_HasRectIntersectionFloat(&playerBox, &enemyBox))
        {
            continue;
        }

        if (enemy->snuffsLantern())
        {
            if (player_.lanternOpen())
            {
                player_.snuffLantern(balance_.mothOilStolen);
                enemy->startFleeing(player_.centre(), balance_.mothFleeSeconds);
            }
        }
        else if (!playerInLight)
        {
            player_.takeHit();  // takeHit ignores further hits while the player is still safe
        }
    }
}

void PlayScene::toggleLantern()
{
    player_.toggleLantern();
}

void PlayScene::lightNearbyLamp()
{
    // Light the closest unlit lamp within reach, if there is one.
    const float reach = balance_.lampReachTiles * kTileSize;
    Lamp* closest = nullptr;
    float closestDistance = reach;
    for (Lamp& lamp : lamps_)
    {
        const float distance = distanceBetween(player_.centre(), lamp.centre());
        if (!lamp.isLit() && distance <= closestDistance)
        {
            closest = &lamp;
            closestDistance = distance;
        }
    }
    if (closest != nullptr)
    {
        closest->light();
    }
}

void PlayScene::collectOilCans()
{
    const SDL_FRect playerBox = player_.bounds();
    for (OilCan& can : oilCans_)
    {
        const SDL_FRect canBox = can.bounds();
        if (!can.isCollected() && SDL_HasRectIntersectionFloat(&playerBox, &canBox))
        {
            can.collect();
            player_.addOil(balance_.oilCanRefill);
        }
    }
}

void PlayScene::refillFromLitLamps(double dt)
{
    // Standing in a lit lamp's pool of light tops up the lantern.
    if (litLampCovering(player_.centre()) != nullptr)
    {
        player_.addOil(balance_.lampRefillPerSecond * static_cast<float>(dt));
    }
}

const Lamp* PlayScene::litLampCovering(SDL_FPoint point) const
{
    const float radius = balance_.lampRadiusTiles * kTileSize * balance_.lightCoreFraction;
    for (const Lamp& lamp : lamps_)
    {
        if (lamp.isLit() && distanceBetween(point, lamp.centre()) <= radius)
        {
            return &lamp;
        }
    }
    return nullptr;
}

EnemySenses PlayScene::sensesFor(const Enemy& enemy) const
{
    EnemySenses senses;
    senses.playerCentre = player_.centre();

    // The lantern dilemma: open, it repels Creepers but every enemy notices the player from
    // further away, and Moths only notice an open lantern at all.
    senses.sightRange = enemy.sightRange(player_.lanternOpen());

    // Light only matters to enemies that run from it.
    if (!enemy.fleesLight())
    {
        return senses;
    }

    const SDL_FPoint position = enemy.centre();
    if (distanceBetween(position, player_.centre()) <= player_.lightRadius() * balance_.lightCoreFraction)
    {
        senses.inLight = true;
        senses.lightCentre = player_.centre();
    }
    else if (const Lamp* lamp = litLampCovering(position))
    {
        senses.inLight = true;
        senses.lightCentre = lamp->centre();
    }
    return senses;
}

int PlayScene::litLampCount() const
{
    int count = 0;
    for (const Lamp& lamp : lamps_)
    {
        if (lamp.isLit())
        {
            ++count;
        }
    }
    return count;
}

SDL_FPoint PlayScene::mapOrigin() const
{
    // Centre the map in the 1280x720 game area. The scrolling camera from the proposal replaces
    // this once districts are bigger than the screen.
    return SDL_FPoint{ (static_cast<float>(kLogicalWidth) - static_cast<float>(level_.width()) * kTileSize) / 2.0f,
                       (static_cast<float>(kLogicalHeight) - static_cast<float>(level_.height()) * kTileSize) / 2.0f };
}

SDL_FRect PlayScene::tileRect(TilePos pos, float inset) const
{
    const SDL_FPoint origin = mapOrigin();
    return SDL_FRect{ origin.x + static_cast<float>(pos.x) * kTileSize + inset,
                      origin.y + static_cast<float>(pos.y) * kTileSize + inset,
                      kTileSize - 2.0f * inset,
                      kTileSize - 2.0f * inset };
}

void PlayScene::render(SDL_Renderer* renderer)
{
    fillScreen(renderer, 8, 9, 14);
    const SDL_FPoint origin = mapOrigin();

    // The ground: walls and floor. These are the colours at full light; the lighting below
    // darkens them, so they are drawn brighter than they will look. Placeholders until the sprites arrive.
    for (int y = 0; y < level_.height(); ++y)
    {
        for (int x = 0; x < level_.width(); ++x)
        {
            if (level_.isWall(x, y))
            {
                fillRect(renderer, tileRect({ x, y }), 140, 122, 150);  // stone
            }
            else
            {
                fillRect(renderer, tileRect({ x, y }), 78, 72, 86);     // cobbles
            }
        }
    }

    for (const OilCan& can : oilCans_)
    {
        can.render(renderer, origin);
    }
    for (const Lamp& lamp : lamps_)
    {
        lamp.render(renderer, origin);
    }

    for (const std::unique_ptr<Enemy>& enemy : enemies_)
    {
        enemy->render(renderer, origin);
    }

    player_.render(renderer, origin);

    // The night: everything above is darkened except inside the lantern's light and lit lamps.
    std::vector<Light> lights;
    if (player_.lightRadius() > 0.0f)
    {
        const SDL_FPoint centre = player_.centre();
        lights.push_back({ { origin.x + centre.x, origin.y + centre.y }, player_.lightRadius(), kLanternColour });
    }
    for (const Lamp& lamp : lamps_)
    {
        if (lamp.isLit())
        {
            const SDL_FPoint centre = lamp.centre();
            lights.push_back({ { origin.x + centre.x, origin.y + centre.y }, balance_.lampRadiusTiles * kTileSize, kLampColour });
        }
    }
    lighting_.render(renderer, lights);

    // With the lantern shut the player is in the dark, so outline them in a cold blue to show
    // where they are and that the lantern is shut (playtest Issue #1).
    if (!player_.lanternOpen())
    {
        const SDL_FRect box = player_.bounds();
        drawOutline(renderer, SDL_FRect{ origin.x + box.x - 3.0f, origin.y + box.y - 3.0f, box.w + 6.0f, box.h + 6.0f },
                    140, 155, 220);
    }

    // Drawn after the lighting so it is always readable.
    drawHud(renderer);
}

void PlayScene::drawHud(SDL_Renderer* renderer) const
{
    constexpr float kTextY = 14.0f;
    constexpr float kBarY = 12.0f;
    constexpr float kBarWidth = 180.0f;
    constexpr float kBarHeight = 12.0f;

    // Below a quarter of a tank the oil bar turns red and LOW OIL flashes (playtest Issue #1).
    constexpr float kLowOil = 25.0f;
    const bool lowOil = player_.oil() < kLowOil;
    drawText(renderer, 24.0f, kTextY, "Oil", 1.0f);
    if (lowOil)
    {
        drawBar(renderer, 56.0f, kBarY, kBarWidth, kBarHeight, player_.oil() / 100.0f, 220, 60, 50);
        if (static_cast<int>(elapsedSeconds_ * 3.0) % 2 == 0)  // on and off three times a second
        {
            drawText(renderer, 56.0f, 28.0f, "LOW OIL", 1.0f);
        }
    }
    else
    {
        drawBar(renderer, 56.0f, kBarY, kBarWidth, kBarHeight, player_.oil() / 100.0f, 214, 140, 50);
    }

    drawText(renderer, 260.0f, kTextY, "Clock", 1.0f);
    drawBar(renderer, 308.0f, kBarY, kBarWidth, kBarHeight,
            static_cast<float>(clockSecondsLeft_) / balance_.clockSeconds, 200, 195, 180);

    // One square per heart: bright while it is still held, dark once lost.
    drawText(renderer, 512.0f, kTextY, "Hearts", 1.0f);
    for (int i = 0; i < balance_.startingHearts; ++i)
    {
        const SDL_FRect heart{ 568.0f + static_cast<float>(i) * 18.0f, kBarY, 12.0f, 12.0f };
        if (i < player_.hearts())
        {
            fillRect(renderer, heart, 214, 64, 64);
        }
        else
        {
            fillRect(renderer, heart, 60, 30, 36);
        }
    }

    drawText(renderer, 648.0f, kTextY,
             std::format("District {}    Lamps {}/{}", district_, litLampCount(), lamps_.size()), 1.0f);

    // The lantern's state as a coloured badge: amber when open, slate blue when shut (playtest Issue #1).
    const bool open = player_.lanternOpen();
    const SDL_FRect badge{ 1000.0f, 7.0f, 160.0f, 22.0f };
    if (open)
    {
        fillRect(renderer, badge, 150, 95, 20);
    }
    else
    {
        fillRect(renderer, badge, 45, 50, 85);
    }
    drawText(renderer, badge.x + 8.0f, badge.y + 5.0f, open ? "LANTERN OPEN" : "LANTERN SHUT", 1.5f);
    drawCentredText(renderer, 700.0f, "WASD/arrows: move    E: light lamp    Space: lantern    Esc/P: pause", 1.0f);
}
