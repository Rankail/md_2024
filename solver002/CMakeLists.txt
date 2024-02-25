project(solver002)

set(SOLVER002_SRC_DIR "${CMAKE_SOURCE_DIR}/solver002/src")

add_executable(solver002
        src/main.cpp
        src/Solver.cpp
)

set_target_output_dirs(solver002)

target_include_directories(solver002 PRIVATE ${SOLVER002_SRC_DIR})

target_link_libraries(solver002 utils)
target_include_directories(solver002 PRIVATE ${MD_UTILS_DIR})