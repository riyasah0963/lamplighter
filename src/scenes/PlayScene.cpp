#include "scenes/PlayScene.h"

#include <cmath>
#include <format>
#include <memory>

#include "Creeper.h"
#include "DrawHelpers.h"
#include "GameConstants.h"
#include "GamePaths.h"
#include "LevelLoader.h"
#include "SceneManager.h"
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

PlayScene::PlayScene(SceneManager& scenes)
    : Scene(scenes)
    , level_(LevelLoader::load(gamePath("levels/district1.txt")))
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
        scenes().requestSwitchTo(std::make_unique<ResultScene>(scenes(), true, elapsedSeconds_));
    }
    else if (player_.hearts() == 0 || clockSecondsLeft_ <= 0.0)
    {
        scenes().requestSwitchTo(std::make_unique<ResultScene>(scenes(), false, elapsedSeconds_));
    }
}

void PlayScene::checkEnemyHits()
{
    // A hit only counts in darkness: an open lantern or a lit lamp protects the player.
    if (player_.lanternOpen() || litLampCovering(player_.centre()) != nullptr)
    {
        return;
    }

    const SDL_FRect playerBox = player_.bounds();
    for (const std::unique_ptr<Enemy>& enemy : enemies_)
    {
        const SDL_FRect enemyBox = enemy->bounds();
        if (SDL_HasRectIntersectionFloat(&playerBox, &enemyBox))
        {
            player_.takeHit();
            return;
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

    // The lantern dilemma: open, it repels enemies but they notice the player from further away.
    const float sightTiles = player_.lanternOpen() ? balance_.creeperSightTiles : balance_.creeperSightTilesLanternShut;
    senses.sightRange = sightTiles * kTileSize;

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

    // Moth start positions, still static until the Moth is added.
    for (const TilePos& moth : level_.objects().mothSpawns)
    {
        fillRect(renderer, tileRect(moth, 10.0f), 225, 205, 255);
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

    // Drawn after the lighting so it is always readable.
    drawHud(renderer);
}

void PlayScene::drawHud(SDL_Renderer* renderer) const
{
    constexpr float kTextY = 14.0f;
    constexpr float kBarY = 12.0f;
    constexpr float kBarWidth = 180.0f;
    constexpr float kBarHeight = 12.0f;

    drawText(renderer, 24.0f, kTextY, "Oil", 1.0f);
    drawBar(renderer, 56.0f, kBarY, kBarWidth, kBarHeight, player_.oil() / 100.0f, 214, 140, 50);

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
             std::format("Lamps {}/{}    Lantern {}", litLampCount(), lamps_.size(), player_.lanternOpen() ? "OPEN" : "SHUT"),
             1.0f);
    drawCentredText(renderer, 700.0f, "WASD/arrows: move    E: light lamp    Space: lantern    Esc/P: pause", 1.0f);
}
