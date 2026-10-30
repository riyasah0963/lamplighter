#pragma once

#include <SDL3/SDL.h>

#include <memory>

class Enemy;

// What an enemy knows about the world this update. PlayScene fills this in, so enemies
// need no knowledge of lamps, lanterns or the player class.
struct EnemySenses
{
    SDL_FPoint playerCentre{};
    float sightRange = 0.0f;    // how close the player must be to be noticed, in map pixels
    bool inLight = false;       // is this enemy standing in the lantern's or a lamp's light?
    SDL_FPoint lightCentre{};   // where that light is, when inLight is true
};

// One behaviour of an enemy (State pattern, after Nystrom (2014) "State", Game Programming
// Patterns, https://gameprogrammingpatterns.com/state.html).
class EnemyState
{
public:
    virtual ~EnemyState() = default;

    // Runs one fixed step and returns the state to change to, or nullptr to stay in this one.
    // Returning the next state, instead of swapping it in here, means a state is never destroyed
    // while its own code is still running.
    virtual std::unique_ptr<EnemyState> update(Enemy& enemy, const EnemySenses& senses, double dt) = 0;
};

// Walks back and forth, turning round at walls, until it notices the player.
class PatrolState final : public EnemyState
{
public:
    std::unique_ptr<EnemyState> update(Enemy& enemy, const EnemySenses& senses, double dt) override;

private:
    float direction_ = 1.0f;  // 1 is right, -1 is left
};

// Heads straight for the player while it can still sense them.
class ChaseState final : public EnemyState
{
public:
    std::unique_ptr<EnemyState> update(Enemy& enemy, const EnemySenses& senses, double dt) override;
};

// Runs away from a light. Keeps running briefly after leaving it, so the enemy does not
// twitch back and forth on the edge of the light.
class FleeState final : public EnemyState
{
public:
    explicit FleeState(SDL_FPoint lightCentre) : lightCentre_(lightCentre) {}

    std::unique_ptr<EnemyState> update(Enemy& enemy, const EnemySenses& senses, double dt) override;

private:
    SDL_FPoint lightCentre_;
    double secondsLeft_ = 0.6;
};
