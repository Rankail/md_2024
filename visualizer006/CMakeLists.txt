project(visualizer006)

set(VISUALIZER006_SRC_DIR "${CMAKE_SOURCE_DIR}/visualizer006/src")

add_executable(visualizer006
    src/main.cpp
    src/Visualizer.cpp
    src/Solver.cpp
)

set_target_output_dirs(visualizer006)

target_include_directories(visualizer006 PRIVATE ${VISUALIZER006_SRC_DIR})

target_link_libraries(visualizer006 utils atlas)
target_include_directories(visualizer006
        PRIVATE ${MD_UTILS_DIR}
        PRIVATE ${MD_ATLAS_DIR}
)

add_dependency(visualizer006 SHARED sdl2)
add_dependency(visualizer006 SHARED sdl2_image)
add_dependency(visualizer006 SHARED sdl2_ttf)
add_dependency(visualizer006 STATIC sdl2main)
add_dependency(visualizer006 SOURCE glad)

add_custom_target(copyAtlasRscVisualizer006 ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${MD_ATLAS_RSC_DIR}"
        "${PROJECT_ARTEFACT_DIR}/${target}/visualizer006/${CMAKE_BUILD_TYPE}/rsc")

add_dependencies(visualizer006 copyAtlasRscVisualizer006)