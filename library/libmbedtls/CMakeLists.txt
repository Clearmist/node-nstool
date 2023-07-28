cmake_minimum_required(VERSION 3.15)
project(libmbedtls)

set(OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../library/libmbedtls)
set(PROJECT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../src/deps/libmbedtls)

file(GLOB SOURCE_FILES ${PROJECT_DIRECTORY}/src/*.c)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME}
    PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${PROJECT_DIRECTORY}/include>
    PRIVATE
    ${PROJECT_DIRECTORY}/src
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY}
)
