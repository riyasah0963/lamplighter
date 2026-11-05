#pragma once

#include "BestTimes.h"
#include "Scene.h"

// Title screen. Enter starts district 1, keys 1 to 3 start that district, Esc quits the game.
// Shows each district's best time from the save file.
class MenuScene : public Scene
{
public:
    explicit MenuScene(SceneManager& scenes);

    void handleEvent(const SDL_Event& event) override;
    void update(double dt) override;
    void render(SDL_Renderer* renderer) override;

private:
    BestTimes bestTimes_;
};
