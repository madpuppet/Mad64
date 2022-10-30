cmake_minimum_required(VERSION 3.10)
project(Mad64 VERSION 2022.06)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_definitions(_CRT_SECURE_NO_WARNINGS)

add_executable(Mad64
	source/application.cpp
	source/appSettings.cpp
	source/cmdManager.cpp
	source/compiler.cpp
	source/contextualHelp.cpp
	source/editWindow.cpp
	source/graphicChunk.cpp
	source/logWindow.cpp
	source/main.cpp
	source/sourcefile.cpp
	source/textInput.cpp
	source/thread.cpp
	source/tinyfiledialogs.c)
	
target_include_directories(Mad64 PUBLIC "Include")
target_link_libraries(Mad64 ../lib/SDL2 ../lib/SDL2_ttf ../lib/SDL2main)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E 
	make_directory $<TARGET_FILE_DIR:${PROJECT_NAME}>/data)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E 
	copy ${CMAKE_CURRENT_SOURCE_DIR}/data/readme.txt $<TARGET_FILE_DIR:${PROJECT_NAME}>/data)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E 
	copy ${CMAKE_CURRENT_SOURCE_DIR}/data/font.otf $<TARGET_FILE_DIR:${PROJECT_NAME}>/data)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E 
	copy ${CMAKE_CURRENT_SOURCE_DIR}/data/music.sid $<TARGET_FILE_DIR:${PROJECT_NAME}>/data)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E 
	copy ${CMAKE_CURRENT_SOURCE_DIR}/data/pscroller.asm $<TARGET_FILE_DIR:${PROJECT_NAME}>/data)

if (WIN32)
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E 
		copy ${CMAKE_CURRENT_SOURCE_DIR}/sdl2.dll $<TARGET_FILE_DIR:${PROJECT_NAME}>)
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E 
		copy ${CMAKE_CURRENT_SOURCE_DIR}/sdl2_ttf.dll $<TARGET_FILE_DIR:${PROJECT_NAME}>)
endif()
	

	