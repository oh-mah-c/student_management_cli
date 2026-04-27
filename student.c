#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "student.h"

StudentArena *arena_init(size_t capacity)
{
    StudentArena *arena = (StudentArena *)malloc(sizeof(StudentArena));
    if (!arena)
        return NULL;

    arena->students = (Student *)malloc(capacity * sizeof(Student));

    if (!arena->students)
    {
        free(arena);
        return NULL;
    }

    arena->capacity = capacity;
    arena->count = 0;
    return arena;
}

void arena_add(StudentArena *arena, int id, const char *name, float gpa)
{
    if (!arena || arena->count >= arena->capacity)
    {
        printf("Error arena overflow");
        return;
    }

    Student *target = arena->students + arena->count;

    target->id = id;
    strncpy(target->name, name, 31);
    target->name[31] = '\0';
    target->gpa = gpa;

    arena->count++;
    printf("Add student successed %p\n", (void *)target);
}

void arena_dump(const StudentArena *arena)
{
    if (!arena || arena->count == 0)
    {
        printf("Arena empty");
        return;
    }

    for (size_t i = 0; i < arena->count; i++)
    {
        Student *curr = arena->students + i;
        printf("[%p] ID: %d | Name: %-15s | GPA: %.1f\n", (void *)curr, curr->id, curr->name, curr->gpa);
    }
}

void arena_free(StudentArena *arena)
{
    if (arena)
    {
        if (arena->students)
        {
            free(arena->students);
            free(arena);
        }
    }
}
