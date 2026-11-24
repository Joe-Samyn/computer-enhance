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

if(MSVC)
    target_compile_definitions(haversine PRIVATE _USE_MATH_DEFINES)
    target_compile_features(haversine PRIVATE cxx_std_20)
    target_compile_features(repition_test PRIVATE cxx_std_20)
endif()
