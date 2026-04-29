#include <stdio.h>
#include <stdlib.h>
#include "student.h"
#include "storage.h"

#define DB_FILE "database.msc"

int main()
{
    StudentArena *arena = arena_init(1000);
    if (!arena)
        return -1;

    db_init_mapping(arena, DB_FILE);

    int choice, tmp_id;
    char tmp_name[32];
    float tmp_gpa;

    while (1)
    {
        printf("Students Management\n");
        printf("1. Add student\n");
        printf("2. Printout Student\n");
        printf("3. Search Student by Id\n");
        printf("4. Save to storage and exit\n");

        if (scanf("%d", &choice) != 1)
        {
            while (getchar() != '\n')
                ;
            continue;
        }

        switch (choice)
        {
        case 1:
            printf("Input ID, Name, GPA\n");
            if (scanf("%d %31s %f", &tmp_id, tmp_name, &tmp_gpa) == 3)
            {
                arena_add(arena, tmp_id, tmp_name, tmp_gpa);
            }
            else
            {
                while (getchar() != '\n')
                    ;
            }
            break;
        case 2:
            arena_dump(arena);
            break;
        case 3:
            printf("Input ID: ");
            if (scanf("%d", &tmp_id) == 1)
            {
                arena_search_id(arena, tmp_id);
            }
            else
            {
                while (getchar() != '\n')
                    ;
            }
            break;
        case 4:
            db_close_mapping(arena);
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