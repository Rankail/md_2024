project(atlas)

set(ATLAS_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/atlas/include")
set(ATLAS_SRC_DIR "${CMAKE_SOURCE_DIR}/atlas/src")
set(ATLAS_RSC_DIR "${CMAKE_SOURCE_DIR}/atlas/rsc")

add_library(atlas
        ${PROJECT_DEPENDENCY_DIR}/glad/src/glad.c # IMPORTANT! Include GLAD.
        src/polygon.cpp
        src/Window.cpp
        src/render/DC.cpp
        src/render/RC.cpp
        src/render/Shader.cpp
        src/render/Texture.cpp
        src/render/buffer/BufferElement.cpp
        src/render/buffer/BufferLayout.cpp
        src/render/buffer/DynamicVertexBuffer.cpp
        src/render/buffer/ElementBuffer.cpp
        src/render/buffer/VertexArray.cpp
        src/render/buffer/VertexBuffer.cpp
        src/render/command/BezierRC.cpp
        src/render/command/CircleRC.cpp
        src/render/command/LineRC.cpp
        src/render/command/PolygonRC.cpp
        src/render/command/PolyLineRC.cpp
        src/render/command/RectRC.cpp
        src/render/command/TextRC.cpp
        src/render/command/TextureRC.cpp
        src/render/font/Font.cpp
        src/render/font/FontFamily.cpp
)

target_include_directories(atlas PRIVATE "${MD_UTILS_DIR}")
target_include_directories(atlas PRIVATE "${ATLAS_INCLUDE_DIR}")

target_include_directories(atlas PUBLIC "${MD_DEPENDENCY_DIR}/spdlog-1.13.0/include")

add_dependency(atlas SHARED sdl2)
add_dependency(atlas SHARED sdl2_image)
add_dependency(atlas SHARED sdl2_ttf)
add_dependency(atlas STATIC sdl2main)
add_dependency(atlas SOURCE glad)
