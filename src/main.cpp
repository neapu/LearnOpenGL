#include <SDL3/SDL.h>
#include <glad/glad.h>
#include "logger.h"

// 窗口尺寸
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

int main(int argc, char* argv[]) {
    // 初始化 SDL
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        NEAPU_LOGE("Failed to initialize SDL: {}", SDL_GetError());
        return -1;
    }

    // 设置 OpenGL 属性
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    // 创建窗口
    SDL_Window* window = SDL_CreateWindow(
        "LearnOpenGL",
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );

    if (!window) {
        NEAPU_LOGE("Failed to create window: {}", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    // 创建 OpenGL 上下文
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        NEAPU_LOGE("Failed to create OpenGL context: {}", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // 初始化 GLAD
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        NEAPU_LOGE("Failed to initialize GLAD");
        SDL_GL_DestroyContext(glContext);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    NEAPU_LOGI("OpenGL Version: {}", (const char*)glGetString(GL_VERSION));
    NEAPU_LOGI("GLSL Version: {}", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
    NEAPU_LOGI("Vendor: {}", (const char*)glGetString(GL_VENDOR));
    NEAPU_LOGI("Renderer: {}", (const char*)glGetString(GL_RENDERER));

    // 启用垂直同步
    SDL_GL_SetSwapInterval(1);

    // 设置视口
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    // 主循环
    bool running = true;
    SDL_Event event;

    NEAPU_LOGI("Entering main loop...");

    while (running) {
        // 处理事件
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    running = false;
                    break;
                case SDL_EVENT_KEY_DOWN:
                    if (event.key.key == SDLK_ESCAPE) {
                        running = false;
                    }
                    break;
                case SDL_EVENT_WINDOW_RESIZED:
                    glViewport(0, 0, event.window.data1, event.window.data2);
                    break;
            }
        }

        // 清除颜色缓冲
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 交换缓冲
        SDL_GL_SwapWindow(window);
    }

    // 清理资源
    NEAPU_LOGI("Cleaning up...");
    SDL_GL_DestroyContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    NEAPU_LOGI("Application terminated successfully");
    return 0;
}
