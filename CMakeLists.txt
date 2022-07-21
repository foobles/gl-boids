cmake_minimum_required(VERSION 3.20)
project(SDL_Glew_Test)

if (NOT MSVC)
    set(CMAKE_CXX_STANDARD 20)
else()
    # Necessary for std::format
    set(CMAKE_CXX_STANDARD 23)
endif()

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

set(SOURCES
        src/main.cpp
        src/sdl_session.cpp
        src/sdl_window.cpp
        src/sdl_image_loader.cpp
        src/gl_session.cpp
        src/gl_shader_program.cpp
        src/obj_format.hpp)

set(SOURCE_HEADERS
        src/sdl_session.hpp
        src/sdl_window.hpp
        src/sdl_image_loader.hpp
        src/gl_session.hpp
        src/gl_shader_program.hpp
        src/obj_format.cpp
        src/mat4.hpp
        src/vec4.hpp src/mesh.hpp src/mesh.cpp)

add_executable(SDL_Glew_Test ${SOURCES} ${SOURCE_HEADERS})
target_include_directories(SDL_Glew_Test PUBLIC ${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS})
target_link_libraries(SDL_Glew_Test PUBLIC ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES})
