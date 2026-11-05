#pragma once

#include "Balance.h"
#include "Enemy.h"
#include "Level.h"

// Drawn to an open lantern from far away and not afraid of light. If it reaches the player it
// snuffs the lantern out and spills some oil, then flutters off. This is the other half of the
// lantern dilemma: an open lantern keeps Creepers away but brings Moths in.
class Moth : public Enemy
{
public:
    Moth(const Level& level, const Balance& balance, TilePos spawn);

    void render(SDL_Renderer* renderer, SDL_FPoint mapOrigin) const override;

    bool fleesLight() const override { return false; }
    bool snuffsLantern() const override { return true; }
};
