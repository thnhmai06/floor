cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# UTF-8
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(/utf-8)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    add_compile_options(-finput-charset=UTF-8)
endif()

project(floor)

# Dependencies
find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)
add_subdirectory(dependencies/SDL3_mixer)
add_subdirectory(dependencies/easing_functions)

# Source files
file(GLOB_RECURSE FLOOR_HEADERS CONFIGURE_DEPENDS include/floor/*.h)
file(GLOB_RECURSE FLOOR_SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_library(${PROJECT_NAME} SHARED ${FLOOR_HEADERS} ${FLOOR_SOURCES})
target_include_directories(${PROJECT_NAME}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link
target_link_libraries(${PROJECT_NAME}
    PUBLIC SDL3::SDL3
    PUBLIC $<IF:$<TARGET_EXISTS:SDL3_image::SDL3_image-shared>,SDL3_image::SDL3_image-shared,SDL3_image::SDL3_image-static>
    PUBLIC SDL3_mixer::SDL3_mixer
    PRIVATE easing_functions
)