project(visualizer001)

set(VISUALIZER001_SRC_DIR "${CMAKE_SOURCE_DIR}/visualizer001/src")

add_executable(visualizer001
    src/main.cpp
    src/Visualizer.cpp
    src/Solver.cpp
)

set_target_output_dirs(visualizer001)

target_include_directories(visualizer001 PRIVATE ${VISUALIZER001_SRC_DIR})

target_link_libraries(visualizer001 utils atlas)
target_include_directories(visualizer001
        PRIVATE ${MD_UTILS_DIR}
        PRIVATE ${MD_ATLAS_DIR}
)

add_dependency(visualizer001 SHARED sdl2)
add_dependency(visualizer001 SHARED sdl2_image)
add_dependency(visualizer001 SHARED sdl2_ttf)
add_dependency(visualizer001 STATIC sdl2main)
add_dependency(visualizer001 SOURCE glad)

add_custom_target(copyAtlasRscVisualizer001 ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${MD_ATLAS_RSC_DIR}"
        "${PROJECT_ARTEFACT_DIR}/${target}/visualizer001/${CMAKE_BUILD_TYPE}/rsc")

add_dependencies(visualizer001 copyAtlasRscVisualizer001)