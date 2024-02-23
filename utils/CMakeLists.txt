project(utils)

set(UTILS_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/utils/include/utils")
set(UTILS_SRC_DIR "${CMAKE_SOURCE_DIR}/utils/src")

add_library(utils
        src/utils.cpp
        src/log/Logger.cpp
        src/reader/InputReader.cpp
        src/reader/OutputReader.cpp
)

target_include_directories(utils PRIVATE "${UTILS_INCLUDE_DIR}")

target_include_directories(utils PUBLIC "${MD_DEPENDENCY_DIR}/spdlog-1.13.0/include")
