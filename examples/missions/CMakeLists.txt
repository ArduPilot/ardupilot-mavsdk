cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(missions)

add_executable(missions
    missions.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(missions
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(missions PRIVATE -Wall -Wextra)
else()
    add_compile_options(missions PRIVATE -WX -W2)
endif()
