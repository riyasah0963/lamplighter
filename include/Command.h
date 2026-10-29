#pragma once

// The things a key press can make happen during play. PlayScene implements these.
class GameActions
{
public:
    virtual ~GameActions() = default;

    virtual void toggleLantern() = 0;
    virtual void lightNearbyLamp() = 0;
};

// Command pattern, after Nystrom (2014) "Command", Game Programming Patterns,
// https://gameprogrammingpatterns.com/command.html
// Each action a key can trigger is an object. InputHandler maps keys to commands, so any key
// can be bound to any action, and keys can be rebound without changing gameplay code.
class Command
{
public:
    virtual ~Command() = default;
    virtual void execute(GameActions& actions) const = 0;
};

class ToggleLanternCommand final : public Command
{
public:
    void execute(GameActions& actions) const override { actions.toggleLantern(); }
};

class LightLampCommand final : public Command
{
public:
    void execute(GameActions& actions) const override { actions.lightNearbyLamp(); }
};
