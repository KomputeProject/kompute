
find_package(Catch2 REQUIRED)

add_executable(test_kompute Main.cpp)

target_link_libraries(test_kompute PRIVATE Catch2::Catch2)
target_link_libraries(test_kompute PRIVATE kompute)

add_test(NAME test_kompute COMMAND test_kompute)


