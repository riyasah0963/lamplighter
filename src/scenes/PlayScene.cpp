#include "scenes/PlayScene.h"

#include <cmath>
#include <format>
#include <memory>

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
{
    for (const TilePos& tile : level_.objects().lamps)
    {
        lamps_.emplace_back(tile);
    }
    for (const TilePos& tile : level_.objects().oilCans)
    {
        oilCans_.emplace_back(tile);
    }
}

void PlayScene::handleEvent(const SDL_Event& event)
{
    if (isKeyPress(event, SDLK_ESCAPE) || isKeyPress(event, SDLK_P))
    {
        scenes().requestPush(std::make_unique<PauseScene>(scenes()));  // Play stays underneath, frozen
    }
    // Test keys until the lose conditions exist (hearts or clock run out).
    else if (isKeyPress(event, SDLK_F1))
    {
        scenes().requestSwitchTo(std::make_unique<ResultScene>(scenes(), true, elapsedSeconds_));
    }
    else if (isKeyPress(event, SDLK_F2))
    {
        scenes().requestSwitchTo(std::make_unique<ResultScene>(scenes(), false, elapsedSeconds_));
    }
    else if (const Command* command = input_.commandFor(event))
    {
        command->execute(*this);  // E lights a lamp, Space opens or shuts the lantern
    }
}

void PlayScene::update(double dt)
{
    elapsedSeconds_ += dt;
    player_.update(dt);
    collectOilCans();
    refillFromLitLamps(dt);

    // Win: every lamp in the district is lit.
    if (litLampCount() == static_cast<int>(lamps_.size()))
    {
        scenes().requestSwitchTo(std::make_unique<ResultScene>(scenes(), true, elapsedSeconds_));
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
    const float lampRadius = balance_.lampRadiusTiles * kTileSize;
    for (const Lamp& lamp : lamps_)
    {
        if (lamp.isLit() && distanceBetween(player_.centre(), lamp.centre()) <= lampRadius)
        {
            player_.addOil(balance_.lampRefillPerSecond * static_cast<float>(dt));
            return;  // one lamp's worth at a time, even where two pools overlap
        }
    }
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

    // Enemy start positions, still static until the enemy step.
    for (const TilePos& creeper : level_.objects().creeperSpawns)
    {
        fillRect(renderer, tileRect(creeper, 6.0f), 150, 120, 220);
    }
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
    drawCentredText(renderer, 12.0f,
                    std::format("Oil {:3.0f}%    Lantern {}    Lamps {}/{}    Time {:.1f} s",
                                player_.oil(), player_.lanternOpen() ? "OPEN" : "SHUT",
                                litLampCount(), lamps_.size(), elapsedSeconds_),
                    1.0f);
    drawCentredText(renderer, 700.0f, "WASD/arrows: move    E: light lamp    Space: lantern    Esc/P: pause", 1.0f);
}
