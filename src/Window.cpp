#include "Window.h"
#include <stdexcept>
#include <logger.h>

Window::Window(int width, int height, const std::string& title)
    : m_width(width), m_height(height), m_title(title)
{
    m_window = SDL_CreateWindow(
        m_title.c_str(),
        m_width,
        m_height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );
    if (!m_window) {
        throw std::runtime_error("Failed to create SDL Window");
    }
}

Window::~Window() {
    if (m_window) {
        SDL_DestroyWindow(m_window);
    }
}

void Window::processEvents(SDL_Event& event)
{
    if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
        NEAPU_LOGI("Window close requested.");
    } else if (event.type == SDL_EVENT_WINDOW_RESIZED) {
        m_width = event.window.data1;
        m_height = event.window.data2;
        NEAPU_LOGI("Window resized to {}x{}", m_width, m_height);
    }
}
