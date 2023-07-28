cmake_minimum_required(VERSION 3.15)
project(tc)

set(OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../library/libtoolchain)
set(PROJECT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../src/deps/libtoolchain)

file(GLOB_RECURSE SOURCE_FILES ${PROJECT_DIRECTORY}/src/*.cpp)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME}
    PUBLIC ../libfmt/build/Release/fmt.lib
    PUBLIC ../libmbedtls/build/Release/libmbedtls.lib
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${PROJECT_DIRECTORY}/include>
    PRIVATE
    ${PROJECT_DIRECTORY}/src
    ${PROJECT_DIRECTORY}/../libfmt/include
    ${PROJECT_DIRECTORY}/../libmbedtls/include
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY}
)
