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

target_link_libraries(LearnOpenGL PRIVATE 
    SDL3::SDL3 
    OpenGL::GL 
    glad 
    logger
)