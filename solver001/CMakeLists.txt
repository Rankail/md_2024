project(solver001)

set(SOLVER001_SRC_DIR "${CMAKE_SOURCE_DIR}/solver001/src")

add_executable(solver001
        src/main.cpp
        src/Solver.cpp
)

set_target_output_dirs(solver001)

target_include_directories(solver001 PRIVATE ${SOLVER001_SRC_DIR})

target_link_libraries(solver001 utils)
target_include_directories(solver001 PRIVATE ${MD_UTILS_DIR})