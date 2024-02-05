cmake_minimum_required(VERSION 3.12)
project(viewer LANGUAGES CXX VERSION 2.0.0)

# --------------------------------------------------------------------------------------------------------
# Setup library
# --------------------------------------------------------------------------------------------------------

add_executable(${PROJECT_NAME})
add_executable(cormon::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20 CXX_EXTENSIONS OFF CXX_STANDARD_REQUIRED ON)

if (UNIX AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror -pedantic -pedantic-errors -Wfatal-errors)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wno-missing-field-initializers)
endif()

# --------------------------------------------------------------------------------------------------------
# Generate "constants" file
# --------------------------------------------------------------------------------------------------------

configure_file("constants.hpp.in" "${CMAKE_CURRENT_SOURCE_DIR}/private/constants.hpp")

# --------------------------------------------------------------------------------------------------------
# Include directories
# --------------------------------------------------------------------------------------------------------

target_include_directories(${PROJECT_NAME} PUBLIC  "include")
target_include_directories(${PROJECT_NAME} PRIVATE "private")
target_include_directories(${PROJECT_NAME} PRIVATE "interface")

# --------------------------------------------------------------------------------------------------------
# Setup sources
# --------------------------------------------------------------------------------------------------------

file(GLOB src "src/*.cpp")
target_sources(${PROJECT_NAME} PRIVATE ${src})

# --------------------------------------------------------------------------------------------------------
# Setup required libraries
# --------------------------------------------------------------------------------------------------------

include("cmake/cpm.cmake")

CPMAddPackage(
  NAME           fmt
  GIT_TAG        10.2.1
  GIT_REPOSITORY "https://github.com/fmtlib/fmt"
)

CPMAddPackage(
  NAME           spdlog
  VERSION        1.12.0
  GIT_REPOSITORY "https://github.com/gabime/spdlog"
  OPTIONS        "SPDLOG_FMT_EXTERNAL ON"
)

CPMFindPackage(
  NAME           tl-expected
  VERSION        1.1.0
  GIT_REPOSITORY "https://github.com/TartanLlama/expected"
)

CPMFindPackage(
  NAME           glaze
  VERSION        2.0.6
  GIT_REPOSITORY "https://github.com/stephenberry/glaze"
)

CPMAddPackage(
  NAME           saucer
  VERSION        2.2.0
  GIT_REPOSITORY "https://github.com/saucer/saucer"
)

CPMAddPackage(
  NAME           reproc
  GIT_TAG        1c07bdbec3f2ecba7125b9499b9a8a77bf9aa8c7
  GIT_REPOSITORY "https://github.com/DaanDeMeyer/reproc"
  OPTIONS        "REPROC++ ON"
)

CPMAddPackage(
  NAME           nfd
  VERSION        1.1.1
  GIT_REPOSITORY "https://github.com/btzy/nativefiledialog-extended"
  OPTIONS        "NFD_PORTAL ON"
)

target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog fmt::fmt saucer::saucer tl::expected glaze::glaze reproc++ nfd)
