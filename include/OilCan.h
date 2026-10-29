#pragma once

#include "Entity.h"
#include "Level.h"

// A can of lamp oil lying in the street. Walking over it refills the lantern once.
class OilCan : public Entity
{
public:
    explicit OilCan(TilePos tile);

    void update(double /*dt*/) override {}
    void render(SDL_Renderer* renderer, SDL_FPoint mapOrigin) const override;

    bool isCollected() const { return collected_; }
    void collect() { collected_ = true; }

private:
    bool collected_ = false;
};
