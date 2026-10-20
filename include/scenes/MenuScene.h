#pragma once

#include "Scene.h"

// Title screen. Enter starts a run; Esc quits the game.
class MenuScene : public Scene
{
public:
    using Scene::Scene;

    void handleEvent(const SDL_Event& event) override;
    void update(double dt) override;
    void render(SDL_Renderer* renderer) override;
};
