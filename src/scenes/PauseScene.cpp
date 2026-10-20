#include "scenes/PauseScene.h"

#include <memory>

#include "DrawHelpers.h"
#include "SceneManager.h"
#include "scenes/MenuScene.h"

void PauseScene::handleEvent(const SDL_Event& event)
{
    if (isKeyPress(event, SDLK_ESCAPE) || isKeyPress(event, SDLK_P))
    {
        scenes().requestPop();  // back to the Play scene underneath, exactly as it was
    }
    else if (isKeyPress(event, SDLK_Q))
    {
        scenes().requestSwitchTo(std::make_unique<MenuScene>(scenes()));  // drops both Pause and Play
    }
}

void PauseScene::update(double /*dt*/)
{
    // Nothing to update; the Play scene below is not updated while paused.
}

void PauseScene::render(SDL_Renderer* renderer)
{
    fillScreen(renderer, 0, 0, 0, 180);  // semi-transparent black dims the Play scene drawn underneath
    drawCentredText(renderer, 420.0f, "PAUSED", 6.0f);
    drawCentredText(renderer, 400.0f, "Esc/P: resume    Q: quit to menu", 2.0f);
}
