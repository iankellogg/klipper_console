cmake_minimum_required(VERSION 3.10)
project(klipper_test)

set(CMAKE_PREFIX_PATH "/usr/local")
set(CMAKE_C_STANDARD 11)#C11
set(CMAKE_CXX_STANDARD 17)#C17
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(INCLUDE_DIRS "D445UI")
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR} ${INCLUDE_DIRS})

# file(GLOB_RECURSE INCLUDES "lv_drivers/*.h"   "lvgl/*.h"  "./*.h" "D445UI/*.h" "D445UI/*/*.h" "lv_fs_if/*.h" "D445UI/*.hpp")
# file(GLOB_RECURSE SOURCES  "lv_drivers/*.c"   "lvgl/*.c" "D445UI/*.c" "D445UI/*/*.c" "lv_fs_if/*.c" "D445UI/*.cpp")


# Proto file

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin) 
SET(CMAKE_CXX_FLAGS "-O3")

# find_package(SDL2 REQUIRED SDL2)
# include_directories(${SDL2_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR})


add_executable(main main.c  ${SOURCES} ${INCLUDES})
add_compile_definitions(LV_CONF_INCLUDE_SIMPLE)
target_link_libraries(main PRIVATE m pthread)
add_custom_target (run COMMAND ${EXECUTABLE_OUTPUT_PATH}/main)