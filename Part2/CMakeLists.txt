cmake_minimum_required(VERSION 4.2)

project(HaversineSolver
        VERSION 1.0.0
        LANGUAGES CXX)

set(SRC src/Main.cpp)

add_executable(haversine ${SRC})