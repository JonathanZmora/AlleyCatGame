cmake_minimum_required(VERSION 3.16)
project(AlleyCatGame)

# Compiler settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS        "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG  "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Executable
add_executable(${PROJECT_NAME}
        src/main.cpp
        src/AlleyCat.cpp
        src/AlleyCat.h
        include/bagel.h
        include/bagel_cfg.h
)

# Include paths for headers
target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/lib/SDL/include
        ${CMAKE_SOURCE_DIR}/lib/SDL_image/include
)

# Fully disable shared builds globally
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)

# SDL3: build static only
set(SDL_STATIC ON CACHE BOOL "Build static SDL3" FORCE)
set(SDL_SHARED OFF CACHE BOOL "Don't build shared SDL3" FORCE)

# SDL3_image: build static only
set(SDL3IMAGE_BUILD_SHARED OFF CACHE BOOL "Don't build shared SDL3_image" FORCE)
set(SDL3IMAGE_BUILD_STATIC ON CACHE BOOL "Build static SDL3_image" FORCE)
set(SDL3IMAGE_INSTALL OFF CACHE BOOL "Disable install for SDL3_image" FORCE)

# SDL3 (static)
set(SDL_STATIC ON)
set(SDL_SHARED OFF)
add_subdirectory(lib/SDL)

# Export the SDL3 build location to SDL3_image
set(SDL3_DIR "${CMAKE_BINARY_DIR}/lib/SDL")

# SDL3_image (static only, no shared)
add_subdirectory(lib/SDL_image)

# Link SDL
target_link_libraries(${PROJECT_NAME} PUBLIC
        SDL3-static
        SDL3_image-static
)

# Box2D (static build, no samples/tests/docs)
set(BOX2D_SAMPLES      OFF)
set(BOX2D_BENCHMARKS   OFF)
set(BOX2D_DOCS         OFF)
set(BOX2D_PROFILE      OFF)
set(BOX2D_VALIDATE     OFF)
set(BOX2D_UNIT_TESTS   OFF)
set(BUILD_SHARED_LIBS  OFF)
add_subdirectory(lib/box2d)

target_link_libraries(${PROJECT_NAME} PUBLIC box2d)

# Copy resources
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/res"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/res"
)

