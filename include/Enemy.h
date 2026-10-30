#pragma once

#include <memory>

#include "EnemyState.h"
#include "Entity.h"
#include "Level.h"

// Something that hunts the player. What it does each step is decided by its current
// EnemyState; this class provides the movement those states use.
class Enemy : public Entity
{
public:
    void update(double dt) override;

    // PlayScene calls this before update(), so the state knows what the enemy can sense.
    void setSenses(const EnemySenses& senses) { senses_ = senses; }

    // Movement for the states. Enemies slide along walls in the same way as the player.
    void moveToward(SDL_FPoint target, double dt);
    void moveAwayFrom(SDL_FPoint point, double dt);
    bool moveSideways(float direction, double dt);  // direction is 1 (right) or -1 (left); true if a wall blocked it

protected:
    Enemy(const Level& level, float speedPixelsPerSecond, float x, float y, float width, float height,
          std::unique_ptr<EnemyState> startState);

private:
    void moveInDirection(float directionX, float directionY, double dt);

    const Level& level_;  // owned by PlayScene, which also owns the enemies
    float speed_;
    std::unique_ptr<EnemyState> state_;
    EnemySenses senses_;
};
