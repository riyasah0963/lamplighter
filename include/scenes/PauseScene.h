#pragma once

#include "Scene.h"

// Drawn over the frozen Play scene. Esc or P resumes; Q goes back to the menu.
class PauseScene : public Scene
{
public:
    using Scene::Scene;

    void handleEvent(const SDL_Event& event) override;
    void update(double dt) override;
    void render(SDL_Renderer* renderer) override;
    bool isOverlay() const override { return true; }
};
