find_package(SDL3 REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(LearnOpenGL 
    main.cpp
    Window.cpp
    Window.h
    OpenGLWindow.cpp
    OpenGLWindow.h
    MyWindow.cpp
    MyWindow.h
    Shader.cpp
    Shader.h
)

# 添加 stb 头文件路径
target_include_directories(LearnOpenGL PRIVATE
    ${CMAKE_SOURCE_DIR}/third_party/stb
)

target_link_libraries(LearnOpenGL PRIVATE 
    SDL3::SDL3 
    OpenGL::GL 
    glad 
    logger
)