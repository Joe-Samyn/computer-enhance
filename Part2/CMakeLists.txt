cmake_minimum_required(VERSION 4.2)

project(HaversineSolver
        VERSION 1.0.0
        LANGUAGES CXX)

set(SRC src/Main.cpp)

add_executable(haversine ${SRC})

target_compile_definitions(haversine PRIVATE PROFILE=1)


# Repition Test build
set(RT_SRC src/RepitionTest.cpp)

add_executable(repition_test ${RT_SRC})
