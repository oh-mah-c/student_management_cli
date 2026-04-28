#include <stdio.h>
#include "storage.h"

void save(const StudentArena *arena, const char *filepath)
{
    if (!arena || arena->count == 0)
    {
        printf("Nothing to save");
        return;
    }

    FILE *file = fopen(filepath, "wb");
    if (!file)
    {
        printf("Can not open file");
        return;
    }

    Database db = {
        .magic = SMC_MAGIC,
        .version = CURRENT_VERSION,
        .count = arena->count};

    fwrite(&db, sizeof(Database), 1, file);
    size_t written = fwrite(arena->students, sizeof(Student), arena->count, file);
    fclose(file);

    if (written == arena->count)
    {
        printf("Dump %zu records into disk success\n", arena->count);
    }
    else
    {
        printf("Fail to dump\n");
    }
}

void load(StudentArena *arena, const char *filepath)
{
    if (!arena)
        return;

    FILE *file = fopen(filepath, "rb");
    if (!file)
    {
        printf("File %s does not exists", filepath);
        return;
    }

    Database db;
    if (fread(&db, sizeof(Database), 1, file) != 1)
    {
        printf("Fail to read db\n");
        fclose(file);
        return;
    }

    if (db.magic != SMC_MAGIC)
    {
        printf("What the heck is this file? Wrong magic number (0x%X)\n", db.magic);
        fclose(file);
        return;
    }

    if (db.count > arena->capacity)
    {
        printf("Can not store\n");
        fclose(file);
        return;
    }

    size_t read_items = fread(arena->students, sizeof(Student), db.count, file);
    fclose(file);

    if (read_items == db.count)
    {
        arena->count = db.count;
        printf("Load items successfully\n");
    }
    else
    {
        printf("Missing data or file broken");
    }
}
