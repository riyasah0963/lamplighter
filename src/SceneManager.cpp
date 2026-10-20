#include "SceneManager.h"

#include <utility>

void SceneManager::requestPush(std::unique_ptr<Scene> scene)
{
    pending_.push_back({ ChangeType::Push, std::move(scene) });
}

void SceneManager::requestPop()
{
    pending_.push_back({ ChangeType::Pop, nullptr });
}

void SceneManager::requestSwitchTo(std::unique_ptr<Scene> scene)
{
    pending_.push_back({ ChangeType::SwitchTo, std::move(scene) });
}

void SceneManager::applyPendingChanges()
{
    // Take the list first, so a request made while these changes run (for example from a
    // scene's destructor) waits for the next call instead of altering the list being looped over.
    std::vector<PendingChange> changes = std::move(pending_);
    pending_.clear();

    for (PendingChange& change : changes)
    {
        switch (change.type)
        {
        case ChangeType::Push:
            if (change.scene)
            {
                scenes_.push_back(std::move(change.scene));
            }
            break;
        case ChangeType::Pop:
            if (!scenes_.empty())
            {
                scenes_.pop_back();
            }
            break;
        case ChangeType::SwitchTo:
            scenes_.clear();
            if (change.scene)
            {
                scenes_.push_back(std::move(change.scene));
            }
            break;
        }
    }
}

void SceneManager::handleEvent(const SDL_Event& event)
{
    if (!scenes_.empty())
    {
        scenes_.back()->handleEvent(event);
    }
}

void SceneManager::update(double dt)
{
    if (!scenes_.empty())
    {
        scenes_.back()->update(dt);
    }
}

void SceneManager::render(SDL_Renderer* renderer)
{
    if (scenes_.empty())
    {
        return;
    }

    // Walk down from the top past any overlays to the first full-screen scene,
    // then draw upwards from there so each overlay lands on top of what is below it.
    std::size_t first = scenes_.size() - 1;
    while (first > 0 && scenes_[first]->isOverlay())
    {
        --first;
    }

    for (std::size_t i = first; i < scenes_.size(); ++i)
    {
        scenes_[i]->render(renderer);
    }
}
