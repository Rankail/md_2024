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
        src/atlas/Visualizer.cpp
        src/atlas/Visualizer.h
        src/atlas/Visualizer.cpp
        src/atlas/Window.cpp
        src/atlas/Window.h
        src/atlas/Hint.h
        src/atlas/WindowFlag.h
        src/atlas/render/DC.cpp
        src/atlas/render/DC.h
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
        src/uid.h
        src/atlas/render/Shader.cpp
        src/atlas/render/RC.cpp
        src/atlas/render/command/TextureRC.cpp
        src/atlas/render/command/CircleRC.cpp
        src/atlas/render/command/CircleRC.h
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