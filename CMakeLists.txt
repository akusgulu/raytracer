cmake_minimum_required(VERSION 3.0)
project(raytracer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -pthread -std=c++11 -O3")

include_directories(raytracer/src
                    raytracer/vendor/pugixml/src
                    )

file(GLOB VENDORS_SOURCES raytracer/vendor/pugixml/src/pugixml.cpp)
file(GLOB PROJECT_HEADERS raytracer/src/*.h)
file(GLOB PROJECT_SOURCES raytracer/src/*.cpp)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS}
                               ${VENDORS_SOURCES})


set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "rtracer")