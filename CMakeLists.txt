cmake_minimum_required(VERSION 3.10)
project(student_management_cli C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
 
add_executable(student_mgmt
    main.c
    student.c
    storage.c
)
