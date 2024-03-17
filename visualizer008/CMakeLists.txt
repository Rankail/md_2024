project(visualizer008)

set(VISUALIZER008_SRC_DIR "${CMAKE_SOURCE_DIR}/visualizer008/src")

add_executable(visualizer008
    src/main.cpp
    src/Visualizer.cpp
    src/Solver.cpp
)

set_target_output_dirs(visualizer008)

target_include_directories(visualizer008 PRIVATE ${VISUALIZER008_SRC_DIR})

target_link_libraries(visualizer008 utils atlas)
target_include_directories(visualizer008
        PRIVATE ${MD_UTILS_DIR}
        PRIVATE ${MD_ATLAS_DIR}
)

add_dependency(visualizer008 SHARED sdl2)
add_dependency(visualizer008 SHARED sdl2_image)
add_dependency(visualizer008 SHARED sdl2_ttf)
add_dependency(visualizer008 STATIC sdl2main)
add_dependency(visualizer008 SOURCE glad)

add_custom_target(copyAtlasRscVisualizer008 ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${MD_ATLAS_RSC_DIR}"
        "${PROJECT_ARTEFACT_DIR}/${target}/visualizer008/${CMAKE_BUILD_TYPE}/rsc")

add_dependencies(visualizer008 copyAtlasRscVisualizer008)