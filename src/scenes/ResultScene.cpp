#include "scenes/ResultScene.h"

#include <format>
#include <memory>

#include "BestTimes.h"
#include "DrawHelpers.h"
#include "GameConstants.h"
#include "GamePaths.h"
#include "SceneManager.h"
#include "scenes/MenuScene.h"
#include "scenes/PlayScene.h"

ResultScene::ResultScene(SceneManager& scenes, int district, bool won, double seconds)
    : Scene(scenes)
    , district_(district)
    , won_(won)
    , seconds_(seconds)
{
    BestTimes bestTimes(gamePath("best_times.txt"));
    if (won_)
    {
        newBest_ = bestTimes.record(district_, seconds_);
    }
    bestSeconds_ = bestTimes.best(district_);
}

bool ResultScene::hasNextDistrict() const
{
    return won_ && district_ < kDistrictCount;
}

void ResultScene::handleEvent(const SDL_Event& event)
{
    if (isKeyPress(event, SDLK_R))
    {
        scenes().requestSwitchTo(PlayScene::start(scenes(), district_));  // retry, as the proposal's fail state describes
    }
    else if (isKeyPress(event, SDLK_N) && hasNextDistrict())
    {
        scenes().requestSwitchTo(PlayScene::start(scenes(), district_ + 1));
    }
    else if (isKeyPress(event, SDLK_RETURN) || isKeyPress(event, SDLK_KP_ENTER))
    {
        scenes().requestSwitchTo(std::make_unique<MenuScene>(scenes()));
    }
}

void ResultScene::update(double /*dt*/)
{
    // Nothing moves on the result screen.
}

void ResultScene::render(SDL_Renderer* renderer)
{
    if (won_)
    {
        fillScreen(renderer, 60, 44, 16);  // warm dawn
        drawCentredText(renderer, 220.0f, "DAWN BREAKS", 6.0f);
        drawCentredText(renderer, 320.0f, district_ == kDistrictCount ? "Every district in Emberfall is lit"
                                                                       : "Every lamp in the district is lit", 2.0f);
    }
    else
    {
        fillScreen(renderer, 30, 8, 14);  // the dark wins
        drawCentredText(renderer, 220.0f, "THE DARK TOOK YOU", 6.0f);
        drawCentredText(renderer, 320.0f, "The shadows swallowed the last light", 2.0f);
    }

    drawCentredText(renderer, 150.0f, std::format("District {}", district_), 3.0f);
    drawCentredText(renderer, 400.0f, std::format("Time: {:.1f} s", seconds_), 3.0f);
    if (newBest_)
    {
        drawCentredText(renderer, 460.0f, "NEW BEST TIME", 2.0f);
    }
    else if (bestSeconds_)
    {
        drawCentredText(renderer, 460.0f, std::format("Best: {:.1f} s", *bestSeconds_), 2.0f);
    }

    drawCentredText(renderer, 600.0f, hasNextDistrict() ? "N: next district    R: retry    Enter: menu"
                                                        : "R: retry    Enter: menu", 2.0f);
}
