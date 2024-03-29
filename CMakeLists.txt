cmake_minimum_required(VERSION 3.10)
project(Mad64 VERSION 2022.06)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (CMAKE_HOST_WIN32)
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

add_compile_definitions(_CRT_SECURE_NO_WARNINGS)

add_executable(Mad64
	source/application.cpp
	source/appSettings.cpp
	source/cmdManager.cpp
	source/compiler.cpp
	source/contextualHelp.cpp
	source/cachedFontRenderer.cpp
	source/editWindow.cpp
	source/graphicChunk.cpp
	source/main.cpp
	source/sourcefile.cpp
	source/thread.cpp
	source/emulatorc64.cpp
	source/tinyfiledialogs.c
	source/chips/c64ram.cpp
	source/chips/cia1.cpp
	source/chips/cia2.cpp
	source/chips/cpu6502.cpp
	source/chips/memc64.cpp
	source/chips/sid.cpp
	source/chips/vic.cpp
	source/dockableManager.cpp
	source/dockableWindow.cpp
	source/dockableWindow_log.cpp
	source/dockableWindow_emulatorScreen.cpp
	source/dockableWindow_searchAndReplace.cpp
	source/dockableWindow_memoryImage.cpp
	source/dockableWindow_memoryDump.cpp
	source/uiItem_textButton.cpp
	source/uiItem_textBox.cpp
	source/uiItem_enumButton.cpp
	
)

if (APPLE)

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

find_library(SDL2TTF_LIBRARY NAMES SDL2_ttf PATHS /opt/homebrew/Cellar/sdl2_ttf/2.20.2/lib)

target_include_directories(Mad64 PUBLIC "include" , "source")
target_link_libraries(Mad64 SDL2::SDL2 ${SDL2TTF_LIBRARY})

else()

target_include_directories(Mad64 PUBLIC "include" , "source")
target_link_directories(Mad64 PUBLIC "lib")
target_link_libraries(Mad64 SDL2 SDL2_ttf SDL2main)

endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E 
	make_directory $<TARGET_FILE_DIR:${PROJECT_NAME}>/samples)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E 
	copy ${CMAKE_CURRENT_SOURCE_DIR}/readme.txt $<TARGET_FILE_DIR:${PROJECT_NAME}>)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E 
	copy ${CMAKE_CURRENT_SOURCE_DIR}/font.ttf $<TARGET_FILE_DIR:${PROJECT_NAME}>)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E 
	copy ${CMAKE_CURRENT_SOURCE_DIR}/font.otf $<TARGET_FILE_DIR:${PROJECT_NAME}>)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E 
	copy ${CMAKE_CURRENT_SOURCE_DIR}/samples/music.sid $<TARGET_FILE_DIR:${PROJECT_NAME}>/samples)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E 
	copy ${CMAKE_CURRENT_SOURCE_DIR}/samples/pscroller.asm $<TARGET_FILE_DIR:${PROJECT_NAME}>/samples)

if (WIN32)
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E 
		copy ${CMAKE_CURRENT_SOURCE_DIR}/sdl2.dll $<TARGET_FILE_DIR:${PROJECT_NAME}>)
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E 
		copy ${CMAKE_CURRENT_SOURCE_DIR}/sdl2_ttf.dll $<TARGET_FILE_DIR:${PROJECT_NAME}>)
endif()
	

	