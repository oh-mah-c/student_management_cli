cmake_minimum_required(VERSION 3.10)
project(student_management_cli C)

set(CMAKE_C_STANDARD 11)

add_executable(student_mgmt
    main.c
    student.c
)
