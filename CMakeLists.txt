cmake_minimum_required(VERSION 3.22)
project(tales_of_legend CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 6.0
    GIT_SHALLOW 1
)
FetchContent_MakeAvailable(raylib)

add_executable(game src/main.cpp)
add_executable(day17
        sandbox/day17/main.cpp
        sandbox/day17/actor.cpp
        sandbox/day17/skill.cpp
        sandbox/day17/battle.cpp
        sandbox/day17/text.cpp

)
target_compile_options(day17 PRIVATE -Wall -Wextra)
target_link_libraries(game PRIVATE raylib)
target_compile_options(game PRIVATE -Wall -Wextra)

option(SANITIZE "Build with ASan and UBSan" OFF)
if(SANITIZE)
    target_compile_options(game PRIVATE
        -fsanitize=address,undefined
        -fno-omit-frame-pointer
    )
    target_link_options(game PRIVATE
        -fsanitize=address,undefined)
endif()
