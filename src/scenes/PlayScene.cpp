#include "scenes/PlayScene.h"

#include <format>
#include <memory>

#include "DrawHelpers.h"
#include "SceneManager.h"
#include "scenes/PauseScene.h"
#include "scenes/ResultScene.h"

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

void PlayScene::render(SDL_Renderer* renderer)
{
    fillScreen(renderer, 20, 24, 36);
    drawCentredText(renderer, 200.0f, "DISTRICT 1  (placeholder)", 4.0f);
    drawCentredText(renderer, 320.0f, std::format("Time: {:.1f} s", elapsedSeconds_), 3.0f);
    drawCentredText(renderer, 600.0f, "Esc/P: pause    F1: win    F2: lose   (test keys)", 2.0f);
}
