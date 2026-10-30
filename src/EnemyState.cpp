#include "EnemyState.h"

#include <cmath>

#include "Enemy.h"

namespace
{
    bool canSensePlayer(const Enemy& enemy, const EnemySenses& senses)
    {
        const SDL_FPoint centre = enemy.centre();
        return std::hypot(senses.playerCentre.x - centre.x, senses.playerCentre.y - centre.y) <= senses.sightRange;
    }
}

std::unique_ptr<EnemyState> PatrolState::update(Enemy& enemy, const EnemySenses& senses, double dt)
{
    if (senses.inLight)
    {
        return std::make_unique<FleeState>(senses.lightCentre);
    }
    if (canSensePlayer(enemy, senses))
    {
        return std::make_unique<ChaseState>();
    }

    if (enemy.moveSideways(direction_, dt))
    {
        direction_ = -direction_;  // hit a wall: turn round
    }
    return nullptr;
}

std::unique_ptr<EnemyState> ChaseState::update(Enemy& enemy, const EnemySenses& senses, double dt)
{
    if (senses.inLight)
    {
        return std::make_unique<FleeState>(senses.lightCentre);
    }
    if (!canSensePlayer(enemy, senses))
    {
        return std::make_unique<PatrolState>();
    }

    enemy.moveToward(senses.playerCentre, dt);
    return nullptr;
}

std::unique_ptr<EnemyState> FleeState::update(Enemy& enemy, const EnemySenses& senses, double dt)
{
    if (senses.inLight)
    {
        lightCentre_ = senses.lightCentre;
    }

    enemy.moveAwayFrom(lightCentre_, dt);

    secondsLeft_ -= dt;
    if (secondsLeft_ <= 0.0 && !senses.inLight)
    {
        return std::make_unique<PatrolState>();
    }
    return nullptr;
}
