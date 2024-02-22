cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 20)

project("md_2024")

set(PROJECT_DEPENDENCY_DIR "${CMAKE_SOURCE_DIR}/deps")
set(PROJECT_ARTEFACT_DIR  "${CMAKE_SOURCE_DIR}/bin")
set(PROJECT_BINARY_DIR "${PROJECT_ARTEFACT_DIR}")

function(set_target_output_dirs target)
    set_target_properties(${target} PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_ARTEFACT_DIR}/${target}/${CMAKE_BUILD_TYPE}/"
            LIBRARY_OUTPUT_DIRECTORY "${PROJECT_ARTEFACT_DIR}/${target}/${CMAKE_BUILD_TYPE}/"
            RUNTIME_OUTPUT_DIRECTORY "${PROJECT_ARTEFACT_DIR}/${target}/${CMAKE_BUILD_TYPE}")
endfunction()

function(add_dependency target type name)
    if (NOT type STREQUAL SHARED AND
            NOT type STREQUAL STATIC AND
            NOT type STREQUAL SOURCE)
        message( FATAL_ERROR "Bad value for argument type. Can be SHARED, STATIC or SOURCE." )
    endif ()

    if (NOT type STREQUAL SOURCE)
        target_include_directories(${target} PRIVATE "${PROJECT_DEPENDENCY_DIR}/${name}/include")
        target_link_directories(${target} PRIVATE "${PROJECT_DEPENDENCY_DIR}/${name}/lib")
        target_link_libraries(${target} ${name})

        if (type STREQUAL SHARED)
            add_custom_command(TARGET ${target}
                    COMMAND ${CMAKE_COMMAND} -E copy
                    "${PROJECT_DEPENDENCY_DIR}/${name}/lib/${name}.dll"
                    "$<TARGET_FILE_DIR:${target}>"
                    COMMENT "Copying files for ${target}...")
        endif ()
    else ()
        file(GLOB deps_${name}_sources CONFIGURE_DEPENDS
                "${PROJECT_DEPENDENCY_DIR}/${name}/src/*.cpp"
                "${PROJECT_DEPENDENCY_DIR}/${name}/src/*.c")

        target_include_directories(${target} PRIVATE "${PROJECT_DEPENDENCY_DIR}/${name}/include")
        target_sources(${target} PRIVATE ${deps_${name}_sources})
    endif ()
endfunction()

if (OPTS_LINK_STDLIB)
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")
    add_compile_options(-static-libgcc -static-libstdc++)
endif ()

add_subdirectory(pointCalc)
add_subdirectory(t001)
