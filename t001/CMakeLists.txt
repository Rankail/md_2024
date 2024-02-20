project(t001)

set(T001_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/t001/include")
set(T001_SRC_DIR "${CMAKE_SOURCE_DIR}/t001/src")
set(T001_RSC_DIR "${CMAKE_SOURCE_DIR}/t001/rsc")

add_executable(t001
        ${PROJECT_DEPENDENCY_DIR}/glad/src/glad.c # IMPORTANT! Include GLAD.
        src/main.cpp
        src/input/InputReader.cpp
        src/Logger.cpp
        src/Logger.h
        src/utils.h
        src/utils.cpp
        src/output/OutputPrinter.cpp
        src/visualize/Visualizer.cpp
        src/visualize/Visualizer.h
        src/visualize/Visualizer.cpp
        src/visualize/Window.cpp
        src/visualize/Window.h
        src/visualize/Hint.h
        src/visualize/WindowFlag.h
        src/visualize/render/DC.cpp
        src/visualize/render/DC.h
        src/visualize/render/Texture.cpp
        src/visualize/render/buffer/BufferElement.cpp
        src/visualize/render/buffer/BufferLayout.cpp
        src/visualize/render/buffer/DynamicVertexBuffer.cpp
        src/visualize/render/buffer/ElementBuffer.cpp
        src/visualize/render/buffer/VertexArray.cpp
        src/visualize/render/buffer/VertexBuffer.cpp
        src/visualize/render/command/BezierRC.cpp
        src/visualize/render/command/LineRC.cpp
        src/visualize/render/command/PolygonRC.cpp
        src/visualize/render/command/PolyLineRC.cpp
        src/visualize/render/command/RectRC.cpp
        src/visualize/render/font/Font.cpp
        src/visualize/render/font/FontFamily.cpp
        src/utils/polygon.cpp
        src/visualize/render/command/TextRC.cpp
        src/uid.h
        src/visualize/render/Shader.cpp
        src/visualize/render/RC.cpp
        src/visualize/render/command/TextureRC.cpp
)

set_target_output_dirs(t001)

target_include_directories(t001 PRIVATE ${T001_SRC_DIR})

target_include_directories(t001 PRIVATE ../deps/spdlog-1.13.0/include)

add_dependency(t001 SHARED sdl2)
add_dependency(t001 SHARED sdl2_image)
add_dependency(t001 SHARED sdl2_ttf)
add_dependency(t001 STATIC sdl2main)
add_dependency(t001 SOURCE glad)

add_custom_target(initTargetDir ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory  "${T001_RSC_DIR}"
            "${PROJECT_ARTEFACT_DIR}/${target}/t001/${CMAKE_BUILD_TYPE}/rsc"
        COMMAND ${CMAKE_COMMAND} -E copy_directory  "${CMAKE_SOURCE_DIR}/../tetris-for-the-win/input_files"
            "${PROJECT_ARTEFACT_DIR}/${target}/t001/${CMAKE_BUILD_TYPE}/input_files"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_ARTEFACT_DIR}/${target}/t001/${CMAKE_BUILD_TYPE}/output_files"
        COMMENT "Initializing target directory ...")

add_dependencies(t001 initTargetDir)