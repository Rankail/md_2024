project(visualizer005)

set(VISUALIZER005_SRC_DIR "${CMAKE_SOURCE_DIR}/visualizer005/src")

add_executable(visualizer005
    src/main.cpp
    src/Visualizer.cpp
    src/Solver.cpp
)

set_target_output_dirs(visualizer005)

target_include_directories(visualizer005 PRIVATE ${VISUALIZER005_SRC_DIR})

target_link_libraries(visualizer005 utils atlas)
target_include_directories(visualizer005
        PRIVATE ${MD_UTILS_DIR}
        PRIVATE ${MD_ATLAS_DIR}
)

add_dependency(visualizer005 SHARED sdl2)
add_dependency(visualizer005 SHARED sdl2_image)
add_dependency(visualizer005 SHARED sdl2_ttf)
add_dependency(visualizer005 STATIC sdl2main)
add_dependency(visualizer005 SOURCE glad)

add_custom_target(copyAtlasRscVisualizer005 ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${MD_ATLAS_RSC_DIR}"
        "${PROJECT_ARTEFACT_DIR}/${target}/visualizer005/${CMAKE_BUILD_TYPE}/rsc")

add_dependencies(visualizer005 copyAtlasRscVisualizer005)