project(pointCalc)

set(POINTCALC_SRC_DIR "${CMAKE_SOURCE_DIR}/pointCalc/src")

add_executable(pointCalc
    src/main.cpp
        ../solver002/src/model2.h
)

set_target_output_dirs(pointCalc)

target_include_directories(pointCalc PRIVATE ${POINTCALC_SRC_DIR})

target_link_libraries(pointCalc utils)
target_include_directories(pointCalc PRIVATE ${MD_UTILS_DIR})
