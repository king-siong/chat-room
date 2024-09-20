enable_testing()

add_library(msg_queue_test_harness_lib STATIC tests/msg_queue_test_harness.cc)
add_library(socket_test_harness_lib STATIC tests/socket_test_harness.cc)
add_library(server_test_harness_lib STATIC tests/server_test_harness.cc)
add_library(codec_test_harness_lib STATIC tests/codec_test_harness.cc)

macro (add_test_exec exec_name)
    add_executable ("${exec_name}" "tests/${exec_name}.cc")
    target_link_libraries ("${exec_name}" ${ARGN})
endmacro (add_test_exec)

add_test_exec(socket_member_function socket_test_harness_lib client_lib server_lib msg_queue_lib tcp_socket_lib)
add_test_exec(socket_p2p socket_test_harness_lib client_lib server_lib msg_queue_lib tcp_socket_lib)

add_test_exec(server_member_function server_test_harness_lib client_lib server_lib msg_queue_lib tcp_socket_lib)
add_test_exec(server_stresses server_test_harness_lib client_lib server_lib msg_queue_lib tcp_socket_lib)

add_test_exec(msg_queue_single_thread msg_queue_lib msg_queue_test_harness_lib)
add_test_exec(msg_queue_multi-producer msg_queue_lib msg_queue_test_harness_lib)
add_test_exec(msg_queue_dead_lock msg_queue_lib msg_queue_test_harness_lib)

add_test_exec(codec_random-msg codec_lib codec_test_harness_lib)
add_test_exec(codec_repeat-msg codec_lib codec_test_harness_lib)
add_test_exec(codec_bonus codec_lib codec_test_harness_lib)

add_test(NAME t_socket_member_function COMMAND socket_member_function)
add_test(NAME t_socket_p2p COMMAND socket_p2p)

add_test(NAME t_server_member_function COMMAND server_member_function)
add_test(NAME t_server_stresses COMMAND server_stresses)

add_test(NAME t_msg_queue_single_thread COMMAND msg_queue_single_thread)
add_test(NAME t_msg_queue_multi-producer COMMAND msg_queue_multi-producer)
add_test(NAME t_msg_queue_dead_lock COMMAND msg_queue_dead_lock)

add_test(NAME t_codec_random-msg COMMAND codec_random-msg)
add_test(NAME t_codec_repeat-msg COMMAND codec_repeat-msg)
add_test(NAME t_codec_bonus COMMAND codec_bonus)

add_custom_target(stage1 COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --timeout 10 -R 't_socket' 
    COMMENT "Testing Stage #1: TCPSocket...")
add_custom_target(stage2 COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --timeout 10 -R 't_server' 
    COMMENT "Testing Stage #2: Server...")
add_custom_target(stage3 COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --timeout 10 -R 't_msg_queue' 
    COMMENT "Testing Stage #3: Message Queue...")
add_custom_target(stage4 COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --timeout 10 -R 't_codec' 
    COMMENT "Testing Stage #4: Codec...")
add_custom_target(all-stage
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --timeout 10 -R 't_socket'
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --timeout 10 -R 't_server'
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --timeout 10 -R 't_msg_queue'
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --timeout 10 -R 't_codec'
    COMMENT "Testing all stages..."
)