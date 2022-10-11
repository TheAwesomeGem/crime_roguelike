cmake_minimum_required(VERSION 3.23)
project(crime_roguelike)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(lib/stduuid)

add_executable(crime_roguelike src/main.cpp src/Delegates.cpp src/Delegates.h)

target_link_libraries(crime_roguelike PUBLIC stduuid)