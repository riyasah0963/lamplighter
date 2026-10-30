#pragma once

#include <SDL3/SDL.h>

#include "Level.h"

// Base class for everything that lives on the map: the player, lamps, oil cans and enemies
// (see the UML sketch). Each entity updates itself once per fixed step and draws itself
// (Update Method pattern).
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

    // Moves along one axis only (one of dx, dy must be 0), then pushes the entity back out of any
    // wall it entered. Returns true if a wall stopped it. Moving X and Y in separate calls lets an
    // entity slide along a wall: if the X move is blocked, the Y move still happens.
    bool moveAlongAxis(const Level& level, float dx, float dy);

    float x_;
    float y_;
    float width_;
    float height_;
};
