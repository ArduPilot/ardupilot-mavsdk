cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(missions_rover)

add_executable(missions_rover
    missions_rover.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(missions_rover
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(missions_rover PRIVATE -Wall -Wextra)
else()
    add_compile_options(missions_rover PRIVATE -WX -W2)
endif()
