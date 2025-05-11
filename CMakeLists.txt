cmake_minimum_required(VERSION 3.28)
project(tetris)

add_executable(tetris main.cpp)
target_link_libraries(tetris raylib winmm)
target_include_directories(tetris PRIVATE SYSTEM raylib-5.0_win64_msvc16/include)
target_link_directories(tetris PRIVATE raylib-5.0_win64_msvc16/lib)