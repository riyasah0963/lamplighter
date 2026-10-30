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
}

Player::Player(const Level& level, const InputHandler& input, const Balance& balance, TilePos start)
    : Entity(static_cast<float>(start.x) * kTileSize + (kTileSize - kHitboxSize) / 2.0f,  // centred in the start tile
             static_cast<float>(start.y) * kTileSize + (kTileSize - kHitboxSize) / 2.0f,
             kHitboxSize, kHitboxSize)
    , level_(level)
    , input_(input)
    , balance_(balance)
    , hearts_(balance.startingHearts)
{
}

void Player::update(double dt)
{
    safeSecondsLeft_ = std::max(0.0, safeSecondsLeft_ - dt);

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

    // At 60 updates per second each step is about 2 pixels, far less than a tile,
    // so the player can never skip through a wall.
    moveAlongAxis(level_, direction.x * distance, 0.0f);
    moveAlongAxis(level_, 0.0f, direction.y * distance);
}

void Player::takeHit()
{
    if (safeSecondsLeft_ > 0.0 || hearts_ == 0)
    {
        return;
    }
    --hearts_;
    safeSecondsLeft_ = balance_.hitSafeSeconds;
}

void Player::render(SDL_Renderer* renderer, SDL_FPoint mapOrigin) const
{
    // Flicker while safe after a hit, so the player can see it happened (on and off 10 times a second).
    if (safeSecondsLeft_ > 0.0 && static_cast<int>(safeSecondsLeft_ * 10.0) % 2 == 0)
    {
        return;
    }

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
