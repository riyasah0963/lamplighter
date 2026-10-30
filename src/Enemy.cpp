#include "Enemy.h"

#include <cmath>
#include <utility>

Enemy::Enemy(const Level& level, float speedPixelsPerSecond, float x, float y, float width, float height,
             std::unique_ptr<EnemyState> startState)
    : Entity(x, y, width, height)
    , level_(level)
    , speed_(speedPixelsPerSecond)
    , state_(std::move(startState))
{
}

void Enemy::update(double dt)
{
    std::unique_ptr<EnemyState> nextState = state_->update(*this, senses_, dt);
    if (nextState)
    {
        state_ = std::move(nextState);
    }
}

void Enemy::moveToward(SDL_FPoint target, double dt)
{
    const SDL_FPoint from = centre();
    moveInDirection(target.x - from.x, target.y - from.y, dt);
}

void Enemy::moveAwayFrom(SDL_FPoint point, double dt)
{
    const SDL_FPoint from = centre();
    moveInDirection(from.x - point.x, from.y - point.y, dt);
}

bool Enemy::moveSideways(float direction, double dt)
{
    return moveAlongAxis(level_, direction * speed_ * static_cast<float>(dt), 0.0f);
}

void Enemy::moveInDirection(float directionX, float directionY, double dt)
{
    const float length = std::hypot(directionX, directionY);
    if (length == 0.0f)
    {
        return;  // already at the target: there is no direction to move in
    }

    // Dividing by the length gives a direction of length 1, so the speed is the same at any angle.
    const float distance = speed_ * static_cast<float>(dt);
    moveAlongAxis(level_, directionX / length * distance, 0.0f);
    moveAlongAxis(level_, 0.0f, directionY / length * distance);
}
