cmake_minimum_required(VERSION 3.19)

project(WAVE_FUNCTION_COLLPASE CXX)

# Set the source files
set(SRCS
	"${CMAKE_CURRENT_LIST_DIR}/src/printer.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/src/image_handler.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/src/wfc.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/src/main.cpp"
)

set(HDRS
	"${CMAKE_CURRENT_LIST_DIR}/include/image_handler.h"
	"${CMAKE_CURRENT_LIST_DIR}/include/stb_image.h"
	"${CMAKE_CURRENT_LIST_DIR}/include/stb_image_write.h"
	"${CMAKE_CURRENT_LIST_DIR}/include/wfc.h"
	"${CMAKE_CURRENT_LIST_DIR}/include/printer.h"
	"${CMAKE_CURRENT_LIST_DIR}/include/utils.h"
)

add_executable(wave_function_collapse_executable "${SRCS}" "${HDRS}")


target_include_directories(wave_function_collapse_executable
	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}/include"
)
