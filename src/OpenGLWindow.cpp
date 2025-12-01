#include "OpenGLWindow.h"
#include <stdexcept>
#include <logger.h>
#include <glad/glad.h>

bool OpenGLWindow::s_gladInitialized = false;

OpenGLWindow::OpenGLWindow(int width, int height, const std::string& title)
    : Window(width, height, title)
{
    m_glContext = SDL_GL_CreateContext(m_window);
    if (!m_glContext) {
        SDL_DestroyWindow(m_window);
        throw std::runtime_error("Failed to create OpenGL context");
    }
    SDL_GL_MakeCurrent(m_window, m_glContext);
    if (!s_gladInitialized) {
        if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
            SDL_GL_DestroyContext(m_glContext);
            SDL_DestroyWindow(m_window);
            throw std::runtime_error("Failed to initialize GLAD");
        }
        s_gladInitialized = true;
    }
    SDL_GL_SetSwapInterval(1);

    // 输出 OpenGL 信息
    NEAPU_LOGI("OpenGL Version: {}", (const char*)glGetString(GL_VERSION));
    NEAPU_LOGI("GLSL Version: {}", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
    NEAPU_LOGI("Vendor: {}", (const char*)glGetString(GL_VENDOR));
    NEAPU_LOGI("Renderer: {}", (const char*)glGetString(GL_RENDERER));
}

OpenGLWindow::~OpenGLWindow()
{
    if (m_glContext) {
        SDL_GL_DestroyContext(m_glContext);
    }
}
