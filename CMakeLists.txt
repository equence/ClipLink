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
    src/core/recent_ids.cpp
)
target_include_directories(cliplink_core PUBLIC src)
target_link_libraries(cliplink_core PUBLIC Qt6::Core)

add_library(cliplink_client_lib
    src/client/clipboard_watcher.cpp
    src/client/clipboard_watcher.h
    src/client/main_window.cpp
    src/client/main_window.h
    src/client/network_client.cpp
    src/client/network_client.h
)
target_include_directories(cliplink_client_lib PUBLIC src)
target_link_libraries(cliplink_client_lib PUBLIC cliplink_core Qt6::Network Qt6::Widgets)

add_library(cliplink_server_lib
    src/server/relay_server.cpp
)
target_include_directories(cliplink_server_lib PUBLIC src)
target_link_libraries(cliplink_server_lib PUBLIC cliplink_core Qt6::Network)

add_executable(cliplink_server
    src/server/main.cpp
)
target_link_libraries(cliplink_server PRIVATE cliplink_server_lib Qt6::Core)

add_executable(cliplink_tests
    tests/test_protocol.cpp
)
target_link_libraries(cliplink_tests PRIVATE cliplink_core Qt6::Test)
add_test(NAME cliplink_tests COMMAND cliplink_tests)

add_executable(cliplink_recent_ids_tests
    tests/test_recent_ids.cpp
)
target_link_libraries(cliplink_recent_ids_tests PRIVATE cliplink_core Qt6::Test)
add_test(NAME cliplink_recent_ids_tests COMMAND cliplink_recent_ids_tests)

add_executable(cliplink_relay_server_tests
    tests/test_relay_server.cpp
)
target_link_libraries(cliplink_relay_server_tests PRIVATE cliplink_server_lib Qt6::Test)
add_test(NAME cliplink_relay_server_tests COMMAND cliplink_relay_server_tests)

add_executable(cliplink_clipboard_tests
    tests/test_clipboard_watcher.cpp
)
target_link_libraries(cliplink_clipboard_tests PRIVATE cliplink_client_lib Qt6::Test)
add_test(NAME cliplink_clipboard_tests COMMAND cliplink_clipboard_tests)

add_executable(cliplink_network_client_tests
    tests/test_network_client.cpp
)
target_link_libraries(cliplink_network_client_tests PRIVATE cliplink_client_lib cliplink_server_lib Qt6::Test)
add_test(NAME cliplink_network_client_tests COMMAND cliplink_network_client_tests)

add_executable(cliplink_main_window_tests
    tests/test_main_window.cpp
)
target_link_libraries(cliplink_main_window_tests PRIVATE cliplink_client_lib Qt6::Test)
add_test(NAME cliplink_main_window_tests COMMAND cliplink_main_window_tests)

add_executable(cliplink_client
    src/client/main.cpp
)
target_link_libraries(cliplink_client PRIVATE cliplink_client_lib)
