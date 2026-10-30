#pragma once

#include "Balance.h"
#include "Enemy.h"
#include "Level.h"

// A shadow from the marsh: patrols, chases the player through the dark and flees from light.
class Creeper : public Enemy
{
public:
    Creeper(const Level& level, const Balance& balance, TilePos spawn);

    void render(SDL_Renderer* renderer, SDL_FPoint mapOrigin) const override;
};
