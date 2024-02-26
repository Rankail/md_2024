project(visualizer003)

set(VISUALIZER003_SRC_DIR "${CMAKE_SOURCE_DIR}/visualizer003/src")

add_executable(visualizer003
    src/main.cpp
    src/Visualizer.cpp
    src/Solver.cpp
)

set_target_output_dirs(visualizer003)

target_include_directories(visualizer003 PRIVATE ${VISUALIZER003_SRC_DIR})

target_link_libraries(visualizer003 utils atlas)
target_include_directories(visualizer003
        PRIVATE ${MD_UTILS_DIR}
        PRIVATE ${MD_ATLAS_DIR}
)

add_dependency(visualizer003 SHARED sdl2)
add_dependency(visualizer003 SHARED sdl2_image)
add_dependency(visualizer003 SHARED sdl2_ttf)
add_dependency(visualizer003 STATIC sdl2main)
add_dependency(visualizer003 SOURCE glad)

add_custom_target(copyAtlasRscVisualizer003 ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${MD_ATLAS_RSC_DIR}"
        "${PROJECT_ARTEFACT_DIR}/${target}/visualizer003/${CMAKE_BUILD_TYPE}/rsc")

add_dependencies(visualizer003 copyAtlasRscVisualizer003)