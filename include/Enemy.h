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

    // How close the player must be to be noticed, in map pixels, with the lantern open or shut.
    float sightRange(bool lanternOpen) const { return lanternOpen ? sightLanternOpen_ : sightLanternShut_; }

    // Creepers run from light; Moths do not.
    virtual bool fleesLight() const { return true; }

    // What touching the player does: false costs a heart in darkness (Creeper), true snuffs an
    // open lantern (Moth).
    virtual bool snuffsLantern() const { return false; }

    // Makes the enemy run away from a point for a while, e.g. a Moth after snuffing the lantern.
    void startFleeing(SDL_FPoint from, double seconds);

protected:
    Enemy(const Level& level, float speedPixelsPerSecond, float sightLanternOpen, float sightLanternShut,
          float x, float y, float width, float height, std::unique_ptr<EnemyState> startState);

private:
    void moveInDirection(float directionX, float directionY, double dt);

    const Level& level_;  // owned by PlayScene, which also owns the enemies
    float speed_;
    float sightLanternOpen_;
    float sightLanternShut_;
    std::unique_ptr<EnemyState> state_;
    EnemySenses senses_;
};
