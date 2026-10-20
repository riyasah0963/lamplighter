#pragma once

#include <SDL3/SDL.h>

#include <memory>
#include <vector>

#include "Scene.h"

// Holds the stack of active scenes (State pattern).
// Only the top scene gets input and updates. Scenes underneath an overlay (Play under Pause)
// are still drawn but stay frozen.
//
// Scenes never change the stack directly. They call a request function, and Game calls
// applyPendingChanges() once the scene's code has finished. Changing the stack straight away
// would destroy a scene while its own handleEvent() or update() is still running.
class SceneManager
{
public:
    void requestPush(std::unique_ptr<Scene> scene);      // put a scene on top, e.g. Pause over Play
    void requestPop();                                   // remove the top scene; popping the last one quits
    void requestSwitchTo(std::unique_ptr<Scene> scene);  // clear the whole stack and start this scene

    void applyPendingChanges();

    void handleEvent(const SDL_Event& event);
    void update(double dt);
    void render(SDL_Renderer* renderer);

    bool empty() const { return scenes_.empty(); }

private:
    enum class ChangeType { Push, Pop, SwitchTo };

    struct PendingChange
    {
        ChangeType type;
        std::unique_ptr<Scene> scene;  // empty for Pop
    };

    std::vector<std::unique_ptr<Scene>> scenes_;  // back() is the top of the stack
    std::vector<PendingChange> pending_;
};
