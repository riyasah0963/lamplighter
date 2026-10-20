#include "scenes/ResultScene.h"

#include <format>
#include <memory>

#include "DrawHelpers.h"
#include "SceneManager.h"
#include "scenes/MenuScene.h"
#include "scenes/PlayScene.h"

ResultScene::ResultScene(SceneManager& scenes, bool won, double seconds)
    : Scene(scenes)
    , won_(won)
    , seconds_(seconds)
{
}

void ResultScene::handleEvent(const SDL_Event& event)
{
    if (isKeyPress(event, SDLK_R))
    {
        scenes().requestSwitchTo(std::make_unique<PlayScene>(scenes()));  // retry, as the proposal's fail state describes
    }
    else if (isKeyPress(event, SDLK_RETURN) || isKeyPress(event, SDLK_KP_ENTER))
    {
        scenes().requestSwitchTo(std::make_unique<MenuScene>(scenes()));
    }
}

void ResultScene::update(double /*dt*/)
{
    // Nothing moves on the result screen.
}

void ResultScene::render(SDL_Renderer* renderer)
{
    if (won_)
    {
        fillScreen(renderer, 60, 44, 16);  // warm dawn
        drawCentredText(renderer, 220.0f, "DAWN BREAKS", 6.0f);
        drawCentredText(renderer, 320.0f, "Every lamp in the district is lit", 2.0f);
    }
    else
    {
        fillScreen(renderer, 30, 8, 14);  // the dark wins
        drawCentredText(renderer, 220.0f, "THE DARK TOOK YOU", 6.0f);
        drawCentredText(renderer, 320.0f, "The shadows swallowed the last light", 2.0f);
    }

    drawCentredText(renderer, 400.0f, std::format("Time: {:.1f} s", seconds_), 3.0f);
    drawCentredText(renderer, 600.0f, "R: retry    Enter: menu", 2.0f);
}
