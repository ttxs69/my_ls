cmake_minimum_required(VERSION 3.10.0)
project(my_ls)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(my_ls 
    main.cxx
    )