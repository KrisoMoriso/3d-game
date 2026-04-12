cmake_minimum_required(VERSION 3.31)
project(3d_game)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(RAYLIB_VERSION 5.5)

find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED
if (NOT raylib_FOUND) # If there's none, fetch and build raylib
    include(FetchContent)
    FetchContent_Declare(
            raylib
            DOWNLOAD_EXTRACT_TIMESTAMP OFF
            URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    )
    FetchContent_GetProperties(raylib)
    if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
        set(FETCHCONTENT_QUIET NO)
        FetchContent_MakeAvailable(raylib)
    endif()
endif()
set(CMAKE_CXX_STANDARD 20)

add_executable(3d_game src/main.cpp
        src/Block.cpp
        src/Block.h
        src/ResourceManager.cpp
        src/ResourceManager.h
        src/Game.cpp
        src/Game.h
        src/Chunk.cpp
        src/Chunk.h
        src/Renderer.cpp
        src/Renderer.h
        src/World.cpp
        src/World.h
        src/ThreadPool.cpp
        src/ThreadPool.h
        src/ray_dda.cpp
        src/ray_dda.h
        src/Player.cpp
        src/Player.h)

target_link_libraries(${PROJECT_NAME} raylib)
