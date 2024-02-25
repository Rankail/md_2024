project(visualizer)

set(VISUALIZER_SRC_DIR "${CMAKE_SOURCE_DIR}/visualizer/src")

add_executable(visualizer
    src/main.cpp
    src/Visualizer.cpp
)

set_target_output_dirs(visualizer)

target_include_directories(visualizer PRIVATE ${VISUALIZER_SRC_DIR})

target_link_libraries(visualizer utils atlas)
target_include_directories(visualizer PRIVATE ${MD_UTILS_DIR})
target_include_directories(visualizer PRIVATE ${MD_ATLAS_DIR})

add_dependency(visualizer SHARED sdl2)
add_dependency(visualizer SHARED sdl2_image)
add_dependency(visualizer SHARED sdl2_ttf)
add_dependency(visualizer STATIC sdl2main)
add_dependency(visualizer SOURCE glad)

add_custom_target(copyAtlasRscVisualizer ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${MD_ATLAS_RSC_DIR}"
        "${PROJECT_ARTEFACT_DIR}/${target}/visualizer/${CMAKE_BUILD_TYPE}/rsc")

add_dependencies(visualizer copyAtlasRscVisualizer)