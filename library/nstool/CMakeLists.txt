cmake_minimum_required(VERSION 3.15)
project(nstool)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(PROJECT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../src)

file(GLOB_RECURSE SOURCE_FILES ${PROJECT_DIRECTORY}/nstool/*.cpp)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../libfmt/build/Release/fmt.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/../liblz4/build/Release/liblz4.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/../libmbedtls/build/Release/libmbedtls.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/../libpietendo/build/Release/pietendo.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/../libtoolchain/build/Release/tc.lib
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${PROJECT_DIRECTORY}/include>
    PRIVATE
    ${PROJECT_DIRECTORY}/nstool
    ${PROJECT_DIRECTORY}/deps/libfmt/include
    ${PROJECT_DIRECTORY}/deps/liblz4/include
    ${PROJECT_DIRECTORY}/deps/libmbedtls/include
    ${PROJECT_DIRECTORY}/deps/libpietendo/include
    ${PROJECT_DIRECTORY}/deps/libtoolchain/include
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
else(MSVC)
    target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
endif(MSVC)
