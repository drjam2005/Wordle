cmake_minimum_required(VERSION 3.28)
project(Wordle)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_CXX_STANDARD 20)

set(exec Wordle)
set(src src/main.cpp)

add_executable(${exec} ${src})

target_include_directories(${exec} PUBLIC
	${CMAKE_SOURCE_DIR}/include
)

if(UNIX)
	target_include_directories(${exec} PUBLIC
		${CMAKE_SOURCE_DIR}/vendor/raylib/unix/include
	)
	target_link_directories(${exec} PUBLIC
		${CMAKE_SOURCE_DIR}/vendor/raylib/unix/lib
	)
	target_link_libraries(${exec} PUBLIC
		raylib
		X11
	)
endif()
