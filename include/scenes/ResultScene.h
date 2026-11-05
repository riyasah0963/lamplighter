#pragma once

#include <optional>

#include "Scene.h"

// End of a run: dawn if every lamp was lit, or the dark won. A win is saved if it is the
// district's best time. R retries the district, N goes on to the next one after a win, and
// Enter goes to the menu.
class ResultScene : public Scene
{
public:
    ResultScene(SceneManager& scenes, int district, bool won, double seconds);

    void handleEvent(const SDL_Event& event) override;
    void update(double dt) override;
    void render(SDL_Renderer* renderer) override;

private:
    bool hasNextDistrict() const;

    int district_;
    bool won_;
    double seconds_;
    std::optional<double> bestSeconds_;  // empty until the district has been won once
    bool newBest_ = false;
};
