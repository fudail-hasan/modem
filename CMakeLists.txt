cmake_minimum_required(VERSION 3.10.0)
project(modem VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(modem
    main.cpp
    modem.h
    modem.cpp
    dictionary.h
    dictionary.cpp
)

include(CTest)
enable_testing()