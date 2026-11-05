#include "scenes/ErrorScene.h"

#include <memory>
#include <utility>
#include <vector>

#include "DrawHelpers.h"
#include "SceneManager.h"
#include "scenes/MenuScene.h"

namespace
{
    // At text scale 2 each character is 16 pixels wide, so 72 characters fit across the screen with a margin.
    constexpr std::size_t kCharsPerLine = 72;

    // Splits a long message into screen-width lines, breaking at the last space that fits
    // (file paths may have no spaces, so a line with none is cut at the full width).
    std::vector<std::string> wrapText(const std::string& text)
    {
        std::vector<std::string> lines;
        std::size_t start = 0;
        while (text.size() - start > kCharsPerLine)
        {
            std::size_t cut = text.rfind(' ', start + kCharsPerLine);
            if (cut == std::string::npos || cut <= start)
            {
                cut = start + kCharsPerLine;
            }
            lines.push_back(text.substr(start, cut - start));
            start = (text[cut] == ' ') ? cut + 1 : cut;
        }
        lines.push_back(text.substr(start));
        return lines;
    }
}

ErrorScene::ErrorScene(SceneManager& scenes, std::string message)
    : Scene(scenes)
    , message_(std::move(message))
{
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", message_.c_str());
}

void ErrorScene::handleEvent(const SDL_Event& event)
{
    if (isKeyPress(event, SDLK_RETURN) || isKeyPress(event, SDLK_KP_ENTER))
    {
        scenes().requestSwitchTo(std::make_unique<MenuScene>(scenes()));
    }
    else if (isKeyPress(event, SDLK_ESCAPE))
    {
        scenes().requestPop();  // the error screen is the only scene, so this quits
    }
}

void ErrorScene::update(double /*dt*/)
{
    // Nothing moves on the error screen.
}

void ErrorScene::render(SDL_Renderer* renderer)
{
    fillScreen(renderer, 40, 12, 12);
    drawCentredText(renderer, 160.0f, "A GAME FILE IS BROKEN", 4.0f);

    float y = 280.0f;
    for (const std::string& line : wrapText(message_))
    {
        drawCentredText(renderer, y, line, 2.0f);
        y += 28.0f;
    }

    drawCentredText(renderer, 560.0f, "Fix the file and try again.", 2.0f);
    drawCentredText(renderer, 640.0f, "Enter: menu    Esc: quit", 2.0f);
}
