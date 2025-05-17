# CREDIT: GITHUB COPILOT WAS USED TO HELP GENERATE THIS CMAKE FILE

cmake_minimum_required(VERSION 3.5)
project(main)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# Set the C++ standard

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

# Enable incomplete features in libc++
add_definitions(-D_LIBCPP_ENABLE_INCOMPLETE_FEATURES)

# the following are installed via vcpkg
find_package(Stb REQUIRED)
find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)
find_package(SDL3_ttf CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(TBB CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS uuid)
find_package(unofficial-enet CONFIG REQUIRED)
find_package(sol2 CONFIG REQUIRED)
find_package(Lua REQUIRED)

# not available in vcpkg
add_subdirectory(modules/FastNoise)

# Common/shared sources
file(GLOB_RECURSE COMMON_SRC_FILES 
    "src/common/*.cpp"
    "src/common/**/*.cpp"
)

# Client-specific
file(GLOB_RECURSE CLIENT_SRC_FILES "src/client/*.cpp")
add_executable(client ${CLIENT_SRC_FILES} ${COMMON_SRC_FILES})

# Server-specific
file(GLOB_RECURSE SERVER_SRC_FILES "src/server/*.cpp")
add_executable(server ${SERVER_SRC_FILES} ${COMMON_SRC_FILES})

target_include_directories(client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_include_directories(server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link everything to your project
target_link_libraries(client SDL3::SDL3-static SDL3_image::SDL3_image-static SDL3_ttf::SDL3_ttf-static glad::glad glm::glm-header-only TBB::tbb FastNoise2 nlohmann_json::nlohmann_json imgui::imgui unofficial::enet::enet Boost::uuid sol2 ${LUA_LIBRARIES})
target_include_directories(client PRIVATE ${Stb_INCLUDE_DIR})
target_include_directories(client PRIVATE ${LUA_INCLUDE_DIR})

add_custom_target(assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/assets ${CMAKE_BINARY_DIR}/assets
)
add_dependencies(client assets)

foreach(dll ${VCPKG_DLLS})
    add_custom_command(TARGET client POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${dll}"
            $<TARGET_FILE_DIR:client>
    )
endforeach()

# === SERVER EXECUTABLE ===

# Link same libraries (you can customize if needed)
target_link_libraries(server
    SDL3::SDL3-static SDL3_image::SDL3_image-static SDL3_ttf::SDL3_ttf-static
    glad::glad glm::glm-header-only TBB::tbb FastNoise2
    nlohmann_json::nlohmann_json imgui::imgui
    sol2
    unofficial::enet::enet Boost::uuid
    ${LUA_LIBRARIES}
)
target_include_directories(server PRIVATE ${Stb_INCLUDE_DIR})
target_include_directories(server PRIVATE ${LUA_INCLUDE_DIR})

# Optionally add asset dependency if server needs it
add_dependencies(server assets)

# Copy required DLLs
foreach(dll ${VCPKG_DLLS})
    add_custom_command(TARGET server POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${dll}"
            $<TARGET_FILE_DIR:server>
    )
endforeach()
