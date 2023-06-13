add_executable(chat_server chat_server.cc)
target_link_libraries(chat_server PRIVATE Drogon::Drogon)
set_output_dir(chat_server examples)

add_executable(chat_client chat_client.cc)
target_link_libraries(chat_client PRIVATE Drogon::Drogon)
set_output_dir(chat_client examples)
