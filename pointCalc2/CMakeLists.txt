project(pointCalc2)

set(POINTCALC2_SRC_DIR "${CMAKE_SOURCE_DIR}/pointCalc2/src")

add_executable(pointCalc2
    src/main.cpp
)

set_target_output_dirs(pointCalc2)

target_include_directories(pointCalc2 PRIVATE ${POINTCALC2_SRC_DIR})

target_link_libraries(pointCalc2 utils)
target_include_directories(pointCalc2 PRIVATE ${MD_UTILS_DIR})
