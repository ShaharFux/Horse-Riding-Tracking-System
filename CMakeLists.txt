cmake_minimum_required(VERSION 3.27)
project(DS1_wet2)

set(CMAKE_CXX_STANDARD 11)

add_executable(DS1_wet2 main.cpp
        plains25a2.cpp
        PlainsImpl.cpp
        PlainsImpl.h
        Team.cpp
        Team.h
        Jockey.cpp
        Jockey.h)
