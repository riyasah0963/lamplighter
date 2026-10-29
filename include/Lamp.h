#pragma once

#include "Entity.h"
#include "Level.h"

// A street lamp. Starts unlit; once the player lights it, it stays lit and its pool of light
// is safe ground that also refills the lantern.
class Lamp : public Entity
{
public:
    explicit Lamp(TilePos tile);

    void update(double /*dt*/) override {}  // a lamp does nothing on its own
    void render(SDL_Renderer* renderer, SDL_FPoint mapOrigin) const override;

    bool isLit() const { return lit_; }
    void light() { lit_ = true; }

private:
    bool lit_ = false;
};
