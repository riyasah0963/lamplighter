#include "scenes/MenuScene.h"

#include <memory>

#include "DrawHelpers.h"
#include "SceneManager.h"
#include "scenes/PlayScene.h"

void MenuScene::handleEvent(const SDL_Event& event)
{
    if (isKeyPress(event, SDLK_RETURN) || isKeyPress(event, SDLK_KP_ENTER))
    {
        scenes().requestSwitchTo(std::make_unique<PlayScene>(scenes()));
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
    drawCentredText(renderer, 240.0f, "LAMPLIGHTER", 6.0f);
    drawCentredText(renderer, 400.0f, "Enter: start    Esc: quit", 2.0f);
    drawCentredText(renderer, 660.0f, "F11: fullscreen", 2.0f);
}
