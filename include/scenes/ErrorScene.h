#pragma once

#include <string>

#include "Scene.h"

// Shown instead of crashing when a level or config file is missing or broken. The message
// names the file and line, so the person who edited it can find the mistake.
// Enter goes back to the menu; Esc quits.
class ErrorScene : public Scene
{
public:
    ErrorScene(SceneManager& scenes, std::string message);

    void handleEvent(const SDL_Event& event) override;
    void update(double dt) override;
    void render(SDL_Renderer* renderer) override;

private:
    std::string message_;
};
