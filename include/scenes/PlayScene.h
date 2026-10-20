#pragma once

#include "Scene.h"

// The district being played. A placeholder for now: it only counts time, so the fixed-step
// update and pausing can be seen working. Level, player and enemies arrive in later steps.
class PlayScene : public Scene
{
public:
    using Scene::Scene;

    void handleEvent(const SDL_Event& event) override;
    void update(double dt) override;
    void render(SDL_Renderer* renderer) override;

private:
    double elapsedSeconds_ = 0.0;  // stops increasing while Pause is on top
};
