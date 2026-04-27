# Student Management CLI

A simple command-line student record manager written in C.

## Requirements

- CMake 3.10+
- A C11-compatible compiler (GCC, Clang, or MSVC)

## Build

```bash
cmake -B build -S .
cmake --build build
```

The executable will be at `build/student_mgmt` (or `build/student_mgmt.exe` on Windows).

## Run

```bash
./build/student_mgmt
```

## Usage

```
Students Management
1. Add student
2. Printout Student
3. Exit
```

Enter the number of your choice and press Enter.

When adding a student, enter ID, name, and GPA separated by spaces:

```
Input ID, Name, GPA
1 Alice 3.8
```
