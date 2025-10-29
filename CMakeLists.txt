cmake_minimum_required(VERSION 3.16)
project(GoGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)


#main execcutable file
add_executable(GoGame "src/main.cpp")

target_include_directories(GoGame PRIVATE
	${CMAKE_SOURCE_DIR}/include
)
target_sources(GoGame PRIVATE 
	"src/Texture.cpp"
	"src/GameLogic.cpp"
  "src/Render.cpp" 
  "src/Helper.cpp")


set(SFML_DIR "${CMAKE_SOURCE_DIR}/SFML/lib/cmake/SFML")

#ensure the folder for accessing assets
target_compile_definitions(GoGame PRIVATE PROJECT_DIR="${CMAKE_SOURCE_DIR}/")

find_package(SFML 3 REQUIRED COMPONENTS Audio Graphics Network Window System)
target_link_libraries(GoGame PRIVATE SFML::Audio SFML::Graphics SFML::Network SFML::Window SFML::System)
