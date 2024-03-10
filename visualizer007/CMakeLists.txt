project(visualizer007)

set(VISUALIZER007_SRC_DIR "${CMAKE_SOURCE_DIR}/visualizer007/src")

add_executable(visualizer007
    src/main.cpp
    src/Visualizer.cpp
    src/Solver.cpp
)

set_target_output_dirs(visualizer007)

target_include_directories(visualizer007 PRIVATE ${VISUALIZER007_SRC_DIR})

target_link_libraries(visualizer007 utils atlas)
target_include_directories(visualizer007
        PRIVATE ${MD_UTILS_DIR}
        PRIVATE ${MD_ATLAS_DIR}
)

add_dependency(visualizer007 SHARED sdl2)
add_dependency(visualizer007 SHARED sdl2_image)
add_dependency(visualizer007 SHARED sdl2_ttf)
add_dependency(visualizer007 STATIC sdl2main)
add_dependency(visualizer007 SOURCE glad)

add_custom_target(copyAtlasRscVisualizer007 ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${MD_ATLAS_RSC_DIR}"
        "${PROJECT_ARTEFACT_DIR}/${target}/visualizer007/${CMAKE_BUILD_TYPE}/rsc")

add_dependencies(visualizer007 copyAtlasRscVisualizer007)