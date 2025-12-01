#include <SDL3/SDL.h>
#include "logger.h"
#include "MyWindow.h"
#include <memory>

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

    auto window = std::make_unique<MyWindow>();
    if (!window->initialize()) {
        NEAPU_LOGE("Failed to initialize window");
        SDL_Quit();
        return -1;
    }

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
            }
            window->processEvents(event);
        }

        window->render();
    }

    // 清理资源（window 析构时会自动清理）
    window.reset();
    SDL_Quit();

    NEAPU_LOGI("Application terminated successfully");
    return 0;
}
