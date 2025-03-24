# CREDIT: GITHUB COPILOT WAS USED TO HELP GENERATE THIS CMAKE FILE

cmake_minimum_required(VERSION 3.5)
project(main)

# Set the C++ standard

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable incomplete features in libc++
add_definitions(-D_LIBCPP_ENABLE_INCOMPLETE_FEATURES)

# Set the name of the executable
set(EXECUTABLE_NAME ${PROJECT_NAME})

set(CMAKE_TOOLCHAIN_FILE "C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake")

set(CMAKE_PREFIX_PATH "C:/vcpkg/installed/x64-windows")

# the following are installed via vcpkg
find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)
find_package(SDL3_ttf CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(TBB CONFIG REQUIRED)

# not available in vcpkg
add_subdirectory(modules/FastNoise)

add_executable(${EXECUTABLE_NAME})

# Link everything to your project
target_link_libraries(${EXECUTABLE_NAME} SDL3::SDL3 SDL3_image::SDL3_image SDL3_ttf::SDL3_ttf glad::glad glm::glm-header-only TBB::tbb FastNoise2)
target_include_directories(${EXECUTABLE_NAME} PRIVATE "C:/vcpkg/installed/x64-windows/include")

add_custom_target(assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/assets ${CMAKE_BINARY_DIR}/assets
)
add_dependencies(${EXECUTABLE_NAME} assets)

# Find all .cpp files automatically
file(GLOB_RECURSE SRC_FILES "src/*.cpp")

# Ensure the headers are available for the compiler
target_include_directories(${EXECUTABLE_NAME} PRIVATE src src/game src/gfx src/util)

# Add your sources to the target
target_sources(${EXECUTABLE_NAME} PRIVATE ${SRC_FILES})

file(GLOB VCPKG_DLLS "C:/dev/vcpkg/installed/x64-windows/bin/*.dll")

foreach(dll ${VCPKG_DLLS})
    add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${dll}"
            $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>
    )
endforeach()
