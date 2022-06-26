cmake_minimum_required(VERSION 3.20)
project(SDL_Glew_Test)

set(CMAKE_CXX_STANDARD 20)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

add_executable(SDL_Glew_Test src/main.cpp)
target_include_directories(SDL_Glew_Test PUBLIC ${SDL2_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS})
target_link_libraries(SDL_Glew_Test PUBLIC ${SDL2_LIBRARIES} ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES})

