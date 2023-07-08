cmake_minimum_required(VERSION 3.25)
project(api C)

set(CMAKE_C_STANDARD 11)

add_executable(api main.c)
add_compile_options(-DEVAL -Wall, Werror -std=gnu11 -02 -pipe -static -s -o Open Open.c -lm)
target_link_libraries(api PRIVATE m)
