cmake_minimum_required(VERSION 3.20)
project(webserv)

set(CMAKE_CXX_STANDARD 11)

add_executable(webserv
        main.cpp
        src/parser_config/Parsefile.cpp
        )
