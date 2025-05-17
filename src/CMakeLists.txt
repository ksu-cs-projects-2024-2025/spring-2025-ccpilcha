file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

# Production core library (no UNIT_TESTING)
add_library(core STATIC ${SRC_FILES})
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(core PUBLIC
    SDL3::SDL3
    SDL3_image::SDL3_image
    SDL3_ttf::SDL3_ttf
    glad::glad
    glm::glm
    TBB::tbb
    FastNoise2
    nlohmann_json::nlohmann_json
    imgui::imgui
    EnTT::EnTT
    Boost::uuid
    sol2
)

# Exclude main.cpp
list(FILTER SRC_FILES EXCLUDE REGEX "main\\.cpp$")
# Test-specific version of core, with UNIT_TESTING
add_library(core_testing STATIC ${SRC_FILES})
target_include_directories(core_testing PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(core_testing PUBLIC
    SDL3::SDL3
    SDL3_image::SDL3_image
    SDL3_ttf::SDL3_ttf
    glad::glad
    glm::glm
    TBB::tbb
    FastNoise2
    nlohmann_json::nlohmann_json
    imgui::imgui
    Boost::uuid
    sol2
)
target_compile_definitions(core_testing PRIVATE UNIT_TESTING)
