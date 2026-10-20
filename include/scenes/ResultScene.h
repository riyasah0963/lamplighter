#pragma once

#include "Scene.h"

// End of a run: dawn if every lamp was lit, or the dark won. R retries the district; Enter goes to the menu.
class ResultScene : public Scene
{
public:
    ResultScene(SceneManager& scenes, bool won, double seconds);

    void handleEvent(const SDL_Event& event) override;
    void update(double dt) override;
    void render(SDL_Renderer* renderer) override;

private:
    bool won_;
    double seconds_;
};
