cmake_minimum_required(VERSION 3.21)

project(ClipLink VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Network Widgets Test)

enable_testing()

add_library(cliplink_core
    src/core/protocol.cpp
)
target_include_directories(cliplink_core PUBLIC src)
target_link_libraries(cliplink_core PUBLIC Qt6::Core)

add_executable(cliplink_tests
    tests/test_protocol.cpp
)
target_link_libraries(cliplink_tests PRIVATE cliplink_core Qt6::Test)
add_test(NAME cliplink_tests COMMAND cliplink_tests)
