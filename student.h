#ifndef STUDENT_H
#define STUDENT_H

#include <stddef.h>

#define DEGREE 3

#pragma pack(push, 1)
typedef struct
{
    int id;        // 4 bytes
    char name[32]; // 32 bytes
    float gpa;     // 4 bytes
} Student;

typedef struct
{
    int is_leaf;
    int num_keys;
    int keys[2 * DEGREE - 1];
    size_t student_idx[2 * DEGREE - 1];
    size_t children_idx[2 * DEGREE];
} BtreeNode;
#pragma pack(pop)

typedef struct
{
    Student *students;
    size_t capacity;
    size_t count;

    BtreeNode *nodes;
    size_t *root_idx;
    size_t *next_student;
    size_t *next_node;
} StudentArena;

StudentArena *arena_init(size_t capacity);
void arena_add(StudentArena *arena, int id, const char *name, float gpa);
void arena_dump(const StudentArena *arena);
void arena_free(StudentArena *arena);
void arena_search_id(StudentArena *arena, int target_id);

#endif