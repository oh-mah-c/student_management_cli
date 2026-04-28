#ifndef STUDENT_H
#define STUDENT_H

#include <stddef.h>

#pragma pack(push, 1)
typedef struct
{
    int id;        // 4 bytes
    char name[32]; // 32 bytes
    float gpa;     // 4 bytes
} Student;
#pragma pack(pop)

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