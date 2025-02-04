# CREDIT: GITHUB COPILOT WAS USED TO HELP GENERATE THIS CMAKE FILE

cmake_minimum_required(VERSION 3.5)
project(main)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Python3 COMPONENTS Interpreter REQUIRED)

# Set optimization level based on build type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
endif()

# Enable incomplete features in libc++
add_definitions(-D_LIBCPP_ENABLE_INCOMPLETE_FEATURES)

# Set the name of the executable
set(EXECUTABLE_NAME ${PROJECT_NAME})

include(FetchContent)

# Fetch Python (install inside build/python)
FetchContent_Declare(
    python
    GIT_REPOSITORY https://github.com/indygreg/python-build-standalone.git
    GIT_TAG 20240107
)

FetchContent_MakeAvailable(python)


# Fetch GLAD
FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG v2.0.4 # Adjust to latest version
	GIT_SHALLOW		TRUE
    SOURCE_SUBDIR  cmake
)

FetchContent_MakeAvailable(glad)

FetchContent_Declare(
	glm
	GIT_REPOSITORY https://github.com/g-truc/glm.git
	GIT_TAG 1.0.1
	GIT_SHALLOW		TRUE
)

FetchContent_MakeAvailable(glm)

# Fetch SDL3 and dependencies only once
FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.2.2 # Replace with the latest stable tag if needed
)

FetchContent_MakeAvailable(SDL3)

# SDL_image
FetchContent_Declare(
    SDL3_image
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
    GIT_TAG release-3.2.0
)
FetchContent_MakeAvailable(SDL3_image)

# SDL_mixer
FetchContent_Declare(
    SDL3_mixer
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
    GIT_TAG origin/main
)
FetchContent_MakeAvailable(SDL3_mixer)

# SDL_ttf
FetchContent_Declare(
    SDL3_ttf
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
    GIT_TAG origin/main
)
FetchContent_MakeAvailable(SDL3_ttf)

# Set up SDL3 and its modules as interface libraries
add_library(SDL3 INTERFACE)
target_include_directories(SDL3 INTERFACE ${SDL3_SOURCE_DIR}/include)
target_link_libraries(SDL3 INTERFACE SDL3::SDL3)

add_library(SDL3_image INTERFACE)
target_include_directories(SDL3_image INTERFACE ${SDL3_image_SOURCE_DIR})
target_link_libraries(SDL3_image INTERFACE SDL3::SDL3 SDL3_image::SDL3_image)

add_library(SDL3_mixer INTERFACE)
target_include_directories(SDL3_mixer INTERFACE ${SDL3_mixer_SOURCE_DIR})
target_link_libraries(SDL3_mixer INTERFACE SDL3::SDL3 SDL3_mixer::SDL3_mixer)

add_library(SDL3_ttf INTERFACE)
target_include_directories(SDL3_ttf INTERFACE ${SDL3_ttf_SOURCE_DIR})
target_link_libraries(SDL3_ttf INTERFACE SDL3::SDL3 SDL3_ttf::SDL3_ttf)


# Define the Python executable inside the build folder
set(PYTHON_EXECUTABLE ${CMAKE_BINARY_DIR}/python/bin/python3)

# Install Jinja2 if missing (Prevents SSL errors)
execute_process(
    COMMAND ${PYTHON_PATH} -m pip install --user --upgrade jinja2 certifi
    RESULT_VARIABLE JINJA2_INSTALLED
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Run GLAD Generator using the correct Python3 version
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/glad/src/glad.c ${CMAKE_BINARY_DIR}/glad/include/glad/gl.h
    COMMAND ${PYTHON_PATH} -m glad
        --out-path ${GLAD_OUT_PATH}
        --api gl:core=4.6 c
    WORKING_DIRECTORY ${glad_SOURCE_DIR}
)

# Ensure the generated files are included
add_library(glad STATIC ${glad_SOURCE_DIR}/src/src/gl.c)
target_include_directories(glad PUBLIC ${glad_SOURCE_DIR}/src/include)

add_executable(${EXECUTABLE_NAME})

# Link everything to your project
target_link_libraries(${EXECUTABLE_NAME} glad SDL3_image SDL3_mixer SDL3_ttf SDL3)
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${glad_SOURCE_DIR}/src/include)

# Add your sources to the target
target_sources(${EXECUTABLE_NAME} 
PRIVATE 
    src/main.cpp
)