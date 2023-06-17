add_cmake_subdirectories()

add_library(id_generator OBJECT id_generator.cc)

add_executable(id_generator_test id_generator_test.cc)
target_link_libraries(id_generator_test id_generator gtest)
add_test(id_generator_test id_generator_test)
