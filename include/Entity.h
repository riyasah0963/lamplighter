#pragma once

#include <SDL3/SDL.h>

// Base class for everything that lives on the map: the player now, and lamps, oil cans and
// enemies later (see the UML sketch). Each entity updates itself once per fixed step and
// draws itself (Update Method pattern).
//
// Positions are in map pixels: (0, 0) is the top-left corner of the map's top-left tile.
class Entity
{
public:
    virtual ~Entity() = default;

    virtual void update(double dt) = 0;

    // mapOrigin is where the map's top-left corner is on screen, so map pixels can be turned into screen pixels.
    virtual void render(SDL_Renderer* renderer, SDL_FPoint mapOrigin) const = 0;

    // The entity's box on the map, used for drawing and collision.
    SDL_FRect bounds() const { return SDL_FRect{ x_, y_, width_, height_ }; }
    SDL_FPoint centre() const { return SDL_FPoint{ x_ + width_ / 2.0f, y_ + height_ / 2.0f }; }

protected:
    Entity(float x, float y, float width, float height)
        : x_(x), y_(y), width_(width), height_(height)
    {
    }

    float x_;
    float y_;
    float width_;
    float height_;
};
