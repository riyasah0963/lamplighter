#include "Player.h"

#include <algorithm>
#include <cmath>

#include "DrawHelpers.h"
#include "GameConstants.h"

namespace
{
    constexpr float kFullTank = 100.0f;

    // Smaller than a tile, so the player fits through 1-tile gaps and does not snag on corners.
    constexpr float kHitboxSize = 20.0f;

    // Which tile column or row a map-pixel coordinate falls in.
    int tileIndex(float pixels)
    {
        return static_cast<int>(std::floor(pixels / kTileSize));
    }
}

Player::Player(const Level& level, const InputHandler& input, const Balance& balance, TilePos start)
    : Entity(static_cast<float>(start.x) * kTileSize + (kTileSize - kHitboxSize) / 2.0f,  // centred in the start tile
             static_cast<float>(start.y) * kTileSize + (kTileSize - kHitboxSize) / 2.0f,
             kHitboxSize, kHitboxSize)
    , level_(level)
    , input_(input)
    , balance_(balance)
{
}

void Player::update(double dt)
{
    // The open lantern burns oil; when the tank runs dry the light goes out.
    if (lanternOpen_)
    {
        oil_ -= kFullTank / balance_.oilFullTankSeconds * static_cast<float>(dt);
        if (oil_ <= 0.0f)
        {
            oil_ = 0.0f;
            lanternOpen_ = false;
        }
    }

    SDL_FPoint direction = input_.movementDirection();

    // Without this, moving diagonally would be about 41% faster than moving straight (Pythagoras: sqrt(1 + 1)).
    if (direction.x != 0.0f && direction.y != 0.0f)
    {
        constexpr float kOneOverRootTwo = 0.70710678f;
        direction.x *= kOneOverRootTwo;
        direction.y *= kOneOverRootTwo;
    }

    const float distance = balance_.playerSpeedTilesPerSecond * kTileSize * static_cast<float>(dt);

    // Moving X and Y separately lets the player slide along a wall: if the X move hits a wall,
    // the Y move still happens. At 60 updates per second each step is about 2 pixels, far less
    // than a tile, so the player can never skip through a wall.
    moveAlongAxis(direction.x * distance, 0.0f);
    moveAlongAxis(0.0f, direction.y * distance);
}

void Player::moveAlongAxis(float dx, float dy)
{
    x_ += dx;
    y_ += dy;

    // The range of tiles the hitbox now overlaps. The tiny margin stops a box that exactly
    // touches the next tile's edge from counting as being inside it.
    constexpr float kEdgeMargin = 0.001f;
    const int left = tileIndex(x_);
    const int right = tileIndex(x_ + width_ - kEdgeMargin);
    const int top = tileIndex(y_);
    const int bottom = tileIndex(y_ + height_ - kEdgeMargin);

    for (int ty = top; ty <= bottom; ++ty)
    {
        for (int tx = left; tx <= right; ++tx)
        {
            if (!level_.isWall(tx, ty))
            {
                continue;
            }

            // Push back out of the wall on the side the player came from.
            if (dx > 0.0f) { x_ = static_cast<float>(tx) * kTileSize - width_; }
            if (dx < 0.0f) { x_ = static_cast<float>(tx + 1) * kTileSize; }
            if (dy > 0.0f) { y_ = static_cast<float>(ty) * kTileSize - height_; }
            if (dy < 0.0f) { y_ = static_cast<float>(ty + 1) * kTileSize; }
        }
    }
}

void Player::render(SDL_Renderer* renderer, SDL_FPoint mapOrigin) const
{
    const SDL_FRect screenRect{ mapOrigin.x + x_, mapOrigin.y + y_, width_, height_ };
    fillRect(renderer, screenRect, 235, 225, 200);  // placeholder until the lamplighter sprite
}

void Player::toggleLantern()
{
    if (lanternOpen_)
    {
        lanternOpen_ = false;
    }
    else if (oil_ > 0.0f)
    {
        lanternOpen_ = true;
    }
}

void Player::addOil(float amount)
{
    oil_ = std::min(oil_ + amount, kFullTank);
}

float Player::lightRadius() const
{
    if (!lanternOpen_)
    {
        return 0.0f;
    }

    // Shrink smoothly from the full-tank radius to the minimum radius as the oil drains.
    const float fullness = oil_ / kFullTank;
    const float radiusTiles = balance_.lanternMinRadiusTiles
                              + (balance_.lanternMaxRadiusTiles - balance_.lanternMinRadiusTiles) * fullness;
    return radiusTiles * kTileSize;
}
