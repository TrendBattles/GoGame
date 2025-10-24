cmake_minimum_required(VERSION 3.10)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")

project(GoGame)

add_executable(GoGame ./src/main.cpp)

include_directories("${CMAKE_SOURCE_DIR}\\SFML\\include")
link_directories("${CMAKE_SOURCE_DIR}\\SFML\\lib")

target_link_libraries(GoGame PRIVATE sfml-audio sfml-graphics sfml-main sfml-network sfml-system sfml-window)