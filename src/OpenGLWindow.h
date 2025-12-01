#pragma once
#include "Window.h"
#include <string>

class OpenGLWindow : public Window {
public:
    OpenGLWindow(int width, int height, const std::string& title);
    virtual ~OpenGLWindow();

protected:
    static bool s_gladInitialized;
    SDL_GLContext m_glContext{nullptr};
};