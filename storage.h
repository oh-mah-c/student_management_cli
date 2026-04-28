#ifndef STORAGE_H
#define STORAGE_H

#include <stdint.h>
#include "student.h"

#define SMC_MAGIC 0x31434D53
#define CURRENT_VERSION 1

#pragma pack(push, 1)
typedef struct
{
    uint32_t magic;
    uint32_t version;
    uint32_t count;
} Database;
#pragma pack(pop)

void save(const StudentArena *arena, const char *filepath);
void load(StudentArena *arena, const char *filepath);

#endif