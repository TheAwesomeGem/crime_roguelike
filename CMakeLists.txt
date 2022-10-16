cmake_minimum_required(VERSION 3.23)
project(crime_roguelike)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(lib/stduuid)

add_executable(crime_roguelike src/Main.cpp src/Delegates.cpp src/Delegates.h src/Math.h src/HashUtil.h src/Common.h src/Typedefs.h src/Zone.cpp src/Zone.h src/Component.cpp src/Component.h src/ComponentStorage.cpp src/ComponentStorage.h src/Entity.cpp src/Entity.h src/EntityStorage.cpp src/EntityStorage.h src/ZoneManager.cpp src/ZoneManager.h src/Command.cpp src/Command.h src/Controller.cpp src/Controller.h src/World.cpp src/World.h src/CommandProcessor.cpp src/CommandProcessor.h src/UpdateManager.cpp src/UpdateManager.h src/GameState.cpp src/GameState.h src/Random.h src/Random.cpp)

target_link_libraries(crime_roguelike PUBLIC stduuid)