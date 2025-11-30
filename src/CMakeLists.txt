find_package(SDL3 REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(LearnOpenGL main.cpp)
target_link_libraries(LearnOpenGL PRIVATE 
    SDL3::SDL3 
    OpenGL::GL 
    glad 
    logger
)