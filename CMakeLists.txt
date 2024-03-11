cmake_minimum_required(VERSION 3.20)

project(lmaocat)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(
        -O3 -fno-tree-vectorize
        -Wall -Wextra -Wpedantic -Wsign-conversion -Wunused-macros
        -Wno-endif-labels -Warith-conversion -Wformat=2 -Werror
)

include_directories(src)


set(HEADERS
        src/Args/Args.hpp
        src/Rainbow/Rainbow.hpp
        src/Rainbow/ANSI/ANSI.hpp
        src/lmaocat/lmaocat.hpp
)

set(SOURCE
        src/main.cpp
        src/Args/Args.cpp
        src/Rainbow/Rainbow.cpp
        src/Rainbow/ANSI/ANSI.cpp
        src/lmaocat/lmaocat.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE} ${HEADERS})
