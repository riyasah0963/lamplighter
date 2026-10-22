#pragma once

#include <SDL3/SDL.h>

#include "Level.h"
#include "Scene.h"

// The district being played. For now it loads the level and draws it as coloured squares, and counts
// time so the fixed-step update and pausing can be seen working. Player and enemies arrive in later steps.
class PlayScene : public Scene
{
public:
    // Loads district 1. Throws LevelLoadError if the level file is missing or broken.
    explicit PlayScene(SceneManager& scenes);

    void handleEvent(const SDL_Event& event) override;
    void update(double dt) override;
    void render(SDL_Renderer* renderer) override;

private:
    // Screen rectangle for a tile, shrunk by `inset` pixels on every side (objects are drawn smaller than tiles).
    SDL_FRect tileRect(TilePos pos, float inset = 0.0f) const;

    Level level_;
    double elapsedSeconds_ = 0.0;  // stops increasing while Pause is on top
};
