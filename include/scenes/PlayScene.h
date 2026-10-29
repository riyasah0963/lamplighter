#pragma once

#include <SDL3/SDL.h>

#include "InputHandler.h"
#include "Level.h"
#include "Player.h"
#include "Scene.h"

// The district being played: loads the level and lets the player walk around it.
// Lamps, oil, lighting and enemies arrive in the next steps.
class PlayScene : public Scene
{
public:
    // Loads district 1. Throws LevelLoadError if the level file is missing or broken.
    explicit PlayScene(SceneManager& scenes);

    void handleEvent(const SDL_Event& event) override;
    void update(double dt) override;
    void render(SDL_Renderer* renderer) override;

private:
    // Where the map's top-left corner is drawn on screen.
    SDL_FPoint mapOrigin() const;

    // Screen rectangle for a tile, shrunk by `inset` pixels on every side (objects are drawn smaller than tiles).
    SDL_FRect tileRect(TilePos pos, float inset = 0.0f) const;

    // Order matters: members are created top to bottom, and the player keeps references to the
    // level and the input handler, so those two must be declared (and so created) before it.
    Level level_;
    InputHandler input_;
    Player player_;
    double elapsedSeconds_ = 0.0;  // stops increasing while Pause is on top
};
