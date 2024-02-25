project(visualizer004)

set(VISUALIZER004_SRC_DIR "${CMAKE_SOURCE_DIR}/visualizer004/src")

add_executable(visualizer004
    src/main.cpp
    src/Visualizer.cpp
    src/Solver.cpp
)

set_target_output_dirs(visualizer004)

target_include_directories(visualizer004 PRIVATE ${VISUALIZER004_SRC_DIR})

target_link_libraries(visualizer004 utils atlas)
target_include_directories(visualizer004
        PRIVATE ${MD_UTILS_DIR}
        PRIVATE ${MD_ATLAS_DIR}
)

add_dependency(visualizer004 SHARED sdl2)
add_dependency(visualizer004 SHARED sdl2_image)
add_dependency(visualizer004 SHARED sdl2_ttf)
add_dependency(visualizer004 STATIC sdl2main)
add_dependency(visualizer004 SOURCE glad)
add_dependency(visualizer004 STATIC box2d)

add_custom_target(copyAtlasRscVisualizer004 ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${MD_ATLAS_RSC_DIR}"
        "${PROJECT_ARTEFACT_DIR}/${target}/visualizer004/${CMAKE_BUILD_TYPE}/rsc")

add_dependencies(visualizer004 copyAtlasRscVisualizer004)