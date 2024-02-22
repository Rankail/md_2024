project(pointCalc)

set(POINTCALC_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/pointCalc/include")
set(POINTCALC_SRC_DIR "${CMAKE_SOURCE_DIR}/pointCalc/src")

add_executable(pointCalc
                src/main.cpp
        src/InputReader.cpp
        src/OutputReader.cpp
        src/OutputReader.h
        src/Logger.cpp
        src/utils.cpp)

set_target_output_dirs(pointCalc)

target_include_directories(pointCalc PRIVATE ${POINTCALC_SRC_DIR})

target_include_directories(pointCalc PRIVATE ../deps/spdlog-1.13.0/include)
