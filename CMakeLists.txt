cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

project(chat_room)

aux_source_directory(server SERVER)
aux_source_directory(client CLIENT)
aux_source_directory(utils UTILS)
aux_source_directory(tests TESTS)
aux_source_directory(demo DEMO)

add_library(msg_queue_lib STATIC utils/msg_queue.cc)
add_library(tcp_socket_lib STATIC utils/tcp_socket.cc)
add_library(codec_lib STATIC utils/codec.cc)
add_library(server_lib STATIC server/server.cc)
add_library(client_lib STATIC client/client.cc)

add_executable(A demo/A.cc ${SERVER} ${UTILS})
add_executable(B demo/B.cc ${CLIENT} ${UTILS})

add_executable(one-time_recv demo/one-time_recv.cc ${CLIENT} ${SERVER} ${UTILS})

add_executable(server demo/server_main.cc ${SERVER} ${UTILS})
add_executable(client demo/client_main.cc ${CLIENT} ${UTILS})

include(tests/tests.cmake)