#pragma once

#include "Balance.h"
#include "Entity.h"
#include "InputHandler.h"
#include "Level.h"

// The lamplighter. Moves in 8 directions, cannot walk through walls, and carries a lantern
// that burns oil while it is open.
class Player : public Entity
{
public:
    // The player keeps references to the level, input and balance values, so all three must
    // outlive it (PlayScene owns them and declares them before the player).
    Player(const Level& level, const InputHandler& input, const Balance& balance, TilePos start);

    void update(double dt) override;
    void render(SDL_Renderer* renderer, SDL_FPoint mapOrigin) const override;

    // Opens or shuts the lantern. It will not open with an empty tank.
    void toggleLantern();
    bool lanternOpen() const { return lanternOpen_; }

    float oil() const { return oil_; }   // 0 (empty) to 100 (full)
    void addOil(float amount);           // never goes above a full tank

    // Radius of the lantern's light in map pixels: shrinks as the oil drains, 0 when shut.
    float lightRadius() const;

    int hearts() const { return hearts_; }

    // Costs one heart, unless the player was hit moments ago and is still safe.
    void takeHit();

private:
    const Level& level_;
    const InputHandler& input_;
    const Balance& balance_;

    bool lanternOpen_ = true;
    float oil_ = 100.0f;
    int hearts_;
    double safeSecondsLeft_ = 0.0;  // above 0 just after a hit, while the player cannot be hit again
};
