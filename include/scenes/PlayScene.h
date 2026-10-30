#pragma once

#include <SDL3/SDL.h>

#include <memory>
#include <vector>

#include "Balance.h"
#include "Command.h"
#include "Enemy.h"
#include "InputHandler.h"
#include "Lamp.h"
#include "Level.h"
#include "LightingSystem.h"
#include "OilCan.h"
#include "Player.h"
#include "Scene.h"

// The district being played: the player walks the dark streets with a lantern, lights the
// lamps and picks up oil while Creepers hunt them. Lighting every lamp wins the district.
class PlayScene : public Scene, public GameActions
{
public:
    // Loads district 1. Throws LevelLoadError if the level file is missing or broken.
    explicit PlayScene(SceneManager& scenes);

    void handleEvent(const SDL_Event& event) override;
    void update(double dt) override;
    void render(SDL_Renderer* renderer) override;

    // GameActions: what the bound keys do (see InputHandler and Command).
    void toggleLantern() override;
    void lightNearbyLamp() override;

private:
    // Where the map's top-left corner is drawn on screen.
    SDL_FPoint mapOrigin() const;

    // Screen rectangle for a tile, shrunk by `inset` pixels on every side.
    SDL_FRect tileRect(TilePos pos, float inset = 0.0f) const;

    void collectOilCans();
    void refillFromLitLamps(double dt);
    void checkEnemyHits();
    int litLampCount() const;

    // The lit lamp whose pool of light covers this map point, or nullptr if none does.
    const Lamp* litLampCovering(SDL_FPoint point) const;

    // What this enemy can sense this update.
    EnemySenses sensesFor(const Enemy& enemy) const;

    void drawHud(SDL_Renderer* renderer) const;

    // Order matters: members are created top to bottom, and the player keeps references to the
    // balance values, level and input handler, so those must be declared (and created) first.
    Balance balance_;
    Level level_;
    InputHandler input_;
    Player player_;
    std::vector<Lamp> lamps_;
    std::vector<OilCan> oilCans_;
    std::vector<std::unique_ptr<Enemy>> enemies_;
    LightingSystem lighting_;
    double clockSecondsLeft_;      // the candle-clock; the district is lost when it reaches 0
    double elapsedSeconds_ = 0.0;  // the run's time; stops increasing while Pause is on top
};
