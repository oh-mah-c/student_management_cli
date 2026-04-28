#include <stdio.h>
#include <stdlib.h>
#include "student.h"
#include "storage.h"

#define DB_FILE "database.msc"

int main()
{
    StudentArena *arena = arena_init(100);
    if (!arena)
        return -1;

    free(arena->students);
    db_init_mapping(arena, DB_FILE);

    int choice;
    int tmp_id;
    char tmp_name[32];
    float tmp_gpa;

    while (1)
    {
        printf("Students Management\n");
        printf("1. Add student\n");
        printf("2. Printout Student\n");
        printf("3. Save to storage and exit\n");

        if (scanf("%d", &choice) != 1)
            break;

        switch (choice)
        {
        case 1:
            printf("Input ID, Name, GPA\n");
            scanf("%d %31s %f", &tmp_id, tmp_name, &tmp_gpa);
            arena_add(arena, tmp_id, tmp_name, tmp_gpa);
            break;
        case 2:
            arena_dump(arena);
            break;
        case 3:
            db_close_mapping(arena);

            arena->students = NULL;

            arena_free(arena);
            printf("System halted. Data saved");

            return 0;
        default:
            printf("What the ?\n");
        }
    }

    arena_free(arena);
    return 0;
}