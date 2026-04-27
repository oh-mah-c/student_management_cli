#ifndef STUDENT_H
#define STUDENT_H

#include <stddef.h>

typedef struct
{
    int id;
    char name[32];
    float gpa;
} Student;

typedef struct
{
    Student *students;
    size_t capacity;
    size_t count;
} StudentArena;

StudentArena *arena_init(size_t capacity);
void arena_add(StudentArena *arena, int id, const char *name, float gpa);
void arena_dump(const StudentArena *arena);
void arena_free(StudentArena *arena);

#endif