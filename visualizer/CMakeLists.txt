project(visualizer)

set(VISUALIZER_SRC_DIR "${CMAKE_SOURCE_DIR}/visualizer/src")
set(VISUALIZER_RSC_DIR "${CMAKE_SOURCE_DIR}/visualizer/rsc")

add_executable(visualizer
        ${PROJECT_DEPENDENCY_DIR}/glad/src/glad.c # IMPORTANT! Include GLAD.
        src/main.cpp
        src/atlas/Visualizer.cpp
        src/atlas/Visualizer.cpp
        src/atlas/Window.cpp
        src/atlas/render/DC.cpp
        src/atlas/render/Texture.cpp
        src/atlas/render/buffer/BufferElement.cpp
        src/atlas/render/buffer/BufferLayout.cpp
        src/atlas/render/buffer/DynamicVertexBuffer.cpp
        src/atlas/render/buffer/ElementBuffer.cpp
        src/atlas/render/buffer/VertexArray.cpp
        src/atlas/render/buffer/VertexBuffer.cpp
        src/atlas/render/command/BezierRC.cpp
        src/atlas/render/command/LineRC.cpp
        src/atlas/render/command/PolygonRC.cpp
        src/atlas/render/command/PolyLineRC.cpp
        src/atlas/render/command/RectRC.cpp
        src/atlas/render/font/Font.cpp
        src/atlas/render/font/FontFamily.cpp
        src/utils/polygon.cpp
        src/atlas/render/command/TextRC.cpp
        src/atlas/render/Shader.cpp
        src/atlas/render/RC.cpp
        src/atlas/render/command/TextureRC.cpp
        src/atlas/render/command/CircleRC.cpp
)

set_target_output_dirs(visualizer)

target_include_directories(visualizer PRIVATE ${VISUALIZER_SRC_DIR})

target_link_libraries(visualizer utils)
target_include_directories(visualizer PRIVATE ${MD_UTILS_DIR})

add_dependency(visualizer SHARED sdl2)
add_dependency(visualizer SHARED sdl2_image)
add_dependency(visualizer SHARED sdl2_ttf)
add_dependency(visualizer STATIC sdl2main)
add_dependency(visualizer SOURCE glad)

add_custom_target(initTargetDir ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory  "${VISUALIZER_RSC_DIR}"
            "${PROJECT_ARTEFACT_DIR}/${target}/visualizer/${CMAKE_BUILD_TYPE}/rsc"
        #COMMAND ${CMAKE_COMMAND} -E copy_directory  "${CMAKE_SOURCE_DIR}/../tetris-for-the-win/input_files"
        #    "${PROJECT_ARTEFACT_DIR}/${target}/visualizer/${CMAKE_BUILD_TYPE}/input_files"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_ARTEFACT_DIR}/${target}/visualizer/${CMAKE_BUILD_TYPE}/output_files"
        COMMENT "Initializing target directory ...")

add_dependencies(visualizer initTargetDir)