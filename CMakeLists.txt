cmake_minimum_required(VERSION 3.7)

project(PacMan LANGUAGES CXX)


find_package(SDL2 REQUIRED)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
		PRIVATE
		main.cpp
		entity.cpp
		entity.h
		ghost.cpp
		ghost.h
		player.cpp
		player.h
		bonus.cpp
		bonus.h
		lvl.cpp
		lvl.h
		overlay.cpp
		overlay.h
		game.h
		game.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})
target_compile_definitions(${PROJECT_NAME} PRIVATE "SDL_MAIN_HANDLED")
target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_LIBRARIES})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/pacman_sprites.bmp
	DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)	

IF (MSVC)
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
ENDIF()
