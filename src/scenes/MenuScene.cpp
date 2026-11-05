#include "scenes/MenuScene.h"

#include <format>
#include <optional>

#include "DrawHelpers.h"
#include "GameConstants.h"
#include "GamePaths.h"
#include "SceneManager.h"
#include "scenes/PlayScene.h"

MenuScene::MenuScene(SceneManager& scenes)
    : Scene(scenes)
    , bestTimes_(gamePath("best_times.txt"))
{
}

void MenuScene::handleEvent(const SDL_Event& event)
{
    if (isKeyPress(event, SDLK_RETURN) || isKeyPress(event, SDLK_KP_ENTER))
    {
        scenes().requestSwitchTo(PlayScene::start(scenes(), 1));
    }
    else if (isKeyPress(event, SDLK_1) || isKeyPress(event, SDLK_2) || isKeyPress(event, SDLK_3))
    {
        const int district = static_cast<int>(event.key.key - SDLK_1) + 1;  // SDLK_1 to SDLK_3 are consecutive
        scenes().requestSwitchTo(PlayScene::start(scenes(), district));
    }
    else if (isKeyPress(event, SDLK_ESCAPE))
    {
        scenes().requestPop();  // the menu is the only scene on the stack, so popping it ends the game
    }
}

void MenuScene::update(double /*dt*/)
{
    // Nothing moves on the menu yet.
}

void MenuScene::render(SDL_Renderer* renderer)
{
    fillScreen(renderer, 12, 14, 28);  // night blue
    drawCentredText(renderer, 280.0f, "LAMPLIGHTER", 6.0f);
    drawCentredText(renderer, 400.0f, "Enter: start    1-3: choose district    Esc: quit", 2.0f);

    for (int district = 1; district <= kDistrictCount; ++district)
    {
        const std::optional<double> best = bestTimes_.best(district);
        const std::string time = best ? std::format("{:.1f} s", *best) : "not yet lit";
        drawCentredText(renderer, 470.0f + static_cast<float>(district - 1) * 30.0f,
                        std::format("District {} best: {}", district, time), 2.0f);
    }
    drawCentredText(renderer, 680.0f, "F11: fullscreen", 2.0f);
}
