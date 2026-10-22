#include "scenes/PlayScene.h"

#include <format>
#include <memory>

#include "DrawHelpers.h"
#include "GameConstants.h"
#include "GamePaths.h"
#include "LevelLoader.h"
#include "SceneManager.h"
#include "scenes/PauseScene.h"
#include "scenes/ResultScene.h"

PlayScene::PlayScene(SceneManager& scenes)
    : Scene(scenes)
    , level_(LevelLoader::load(gamePath("levels/district1.txt")))
{
}

void PlayScene::handleEvent(const SDL_Event& event)
{
    if (isKeyPress(event, SDLK_ESCAPE) || isKeyPress(event, SDLK_P))
    {
        scenes().requestPush(std::make_unique<PauseScene>(scenes()));  // Play stays underneath, frozen
    }
    // Test keys until real win and lose conditions exist (all lamps lit / hearts or clock run out).
    // F1/F2 rather than letters, because W and other letters will be needed for movement.
    else if (isKeyPress(event, SDLK_F1))
    {
        scenes().requestSwitchTo(std::make_unique<ResultScene>(scenes(), true, elapsedSeconds_));
    }
    else if (isKeyPress(event, SDLK_F2))
    {
        scenes().requestSwitchTo(std::make_unique<ResultScene>(scenes(), false, elapsedSeconds_));
    }
}

void PlayScene::update(double dt)
{
    elapsedSeconds_ += dt;
}

SDL_FRect PlayScene::tileRect(TilePos pos, float inset) const
{
    // Centre the map in the 1280x720 game area. The scrolling camera from the proposal replaces
    // this once districts are bigger than the screen.
    const float mapLeft = (static_cast<float>(kLogicalWidth) - static_cast<float>(level_.width()) * kTileSize) / 2.0f;
    const float mapTop = (static_cast<float>(kLogicalHeight) - static_cast<float>(level_.height()) * kTileSize) / 2.0f;

    return SDL_FRect{ mapLeft + static_cast<float>(pos.x) * kTileSize + inset,
                      mapTop + static_cast<float>(pos.y) * kTileSize + inset,
                      kTileSize - 2.0f * inset,
                      kTileSize - 2.0f * inset };
}

void PlayScene::render(SDL_Renderer* renderer)
{
    fillScreen(renderer, 8, 9, 14);

    // The ground: walls and floor. Placeholder colours until the sprites and lighting arrive.
    for (int y = 0; y < level_.height(); ++y)
    {
        for (int x = 0; x < level_.width(); ++x)
        {
            if (level_.isWall(x, y))
            {
                fillRect(renderer, tileRect({ x, y }), 58, 52, 70);
            }
            else
            {
                fillRect(renderer, tileRect({ x, y }), 24, 26, 36);
            }
        }
    }

    // Starting positions of everything in the district, drawn as smaller coloured squares.
    const LevelObjects& objects = level_.objects();
    for (const TilePos& lamp : objects.lamps)
    {
        fillRect(renderer, tileRect(lamp, 8.0f), 120, 104, 60);  // dull brass: unlit lamp
    }
    for (const TilePos& oilCan : objects.oilCans)
    {
        fillRect(renderer, tileRect(oilCan, 10.0f), 200, 120, 40);
    }
    for (const TilePos& creeper : objects.creeperSpawns)
    {
        fillRect(renderer, tileRect(creeper, 6.0f), 150, 120, 220);
    }
    for (const TilePos& moth : objects.mothSpawns)
    {
        fillRect(renderer, tileRect(moth, 10.0f), 225, 205, 255);
    }
    fillRect(renderer, tileRect(objects.playerStart, 6.0f), 235, 225, 200);

    drawCentredText(renderer, 12.0f, std::format("District 1    Time {:.1f} s    Esc/P: pause    F1: win    F2: lose", elapsedSeconds_), 1.0f);
}
