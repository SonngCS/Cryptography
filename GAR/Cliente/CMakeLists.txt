cmake_minimum_required(VERSION 3.28)
project(Cliente)

set(CMAKE_CXX_STANDARD 20)

set(my_includes "./include/client.h"
        "./include/FileData.h")
set(my_cpps "./src/client.cpp"
        "./src/FileData.cpp")
add_executable(yourExec ${my_includes} ${my_cpps})
include_directories("./include")
target_link_libraries(yourExec ws2_32)