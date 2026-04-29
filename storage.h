#ifndef STORAGE_H
#define STORAGE_H

#include <stdint.h>
#include "student.h"

#define SMC_MAGIC 0x31434D53
#define CURRENT_VERSION 2

#pragma pack(push, 1)
typedef struct
{
    uint32_t magic;
    uint32_t version;
    size_t max_capacity;
    size_t root_node_idx;
    size_t next_student_idx;
    size_t next_node_idx;
} Database;
#pragma pack(pop)

void db_init_mapping(StudentArena *arena, const char *filepath);
void db_close_mapping(StudentArena *arena);

#endif