#pragma once

#include "Entity.h"
#include "InputHandler.h"
#include "Level.h"

// The lamplighter. Moves in 8 directions and cannot walk through walls.
class Player : public Entity
{
public:
    // The player keeps references to the level and input, so both must outlive it
    // (PlayScene owns all three and declares the level and input first).
    Player(const Level& level, const InputHandler& input, TilePos start);

    void update(double dt) override;
    void render(SDL_Renderer* renderer, SDL_FPoint mapOrigin) const override;

private:
    // Moves along one axis only (one of dx, dy is 0), then pushes the player back out of any wall it entered.
    void moveAlongAxis(float dx, float dy);

    const Level& level_;
    const InputHandler& input_;
};
