cmake_minimum_required(VERSION 3.10)
project(MatMult_LPII)

set(CMAKE_C_STANDARD 11)

# Flags de compilação rigorosas (O2 para medição de desempenho)
add_compile_options(-O2 -Wall -Wextra -g)

# Encontra e linka a biblioteca de threads do sistema (pthreads)
find_package(Threads REQUIRED)

add_executable(meu_exe src/main.c)
target_link_libraries(meu_exe PRIVATE Threads::Threads)