cmake_minimum_required(VERSION 3.15)
project(pietendo)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../library/libpietendo)
set(PROJECT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../src/deps/libpietendo)

file(GLOB_RECURSE SOURCE_FILES ${PROJECT_DIRECTORY}/src/*.cpp)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../libfmt/build/Release/fmt.lib
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../libtoolchain/build/Release/tc.lib
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${PROJECT_DIRECTORY}/include>
    PRIVATE
    ${PROJECT_DIRECTORY}/src
    ${PROJECT_DIRECTORY}/../libfmt/include
    ${PROJECT_DIRECTORY}/../libtoolchain/include
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY}
)
