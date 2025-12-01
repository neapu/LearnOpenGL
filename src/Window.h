#pragma once
#include <SDL3/SDL.h>
#include <string>

class Window {
public:
    Window(int width, int height, const std::string& title);
    virtual ~Window();

    virtual bool initialize() { return true; };
    virtual void processEvents(SDL_Event& event);
    virtual void render() {};
    virtual void cleanup() {};

protected:
    SDL_Window* m_window{nullptr};
    int m_width;
    int m_height;
    std::string m_title;
};