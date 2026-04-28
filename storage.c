#include <stdio.h>
#include <stdlib.h>
#include "storage.h"

#ifdef _WIN32
#include <windows.h>
static HANDLE hFile = INVALID_HANDLE_VALUE;
static HANDLE hMap = NULL;
static void *mapped_data = NULL;
static size_t mapped_size = 0;
#else
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
static int fd = -1;
static void *mapped_data = MAP_FAILED;
static size_t mapped_size = 0;
#endif

void db_init_mapping(StudentArena *arena, const char *filepath)
{
    if (!arena)
        return;

    mapped_size = sizeof(Database) + (arena->capacity * sizeof(Student));

#ifdef _WIN32
    hFile = CreateFileA(filepath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        printf("Win32: Can not open file\n");
        return;
    }

    hMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, (DWORD)mapped_size, NULL);
    if (!hMap)
    {
        printf("Win32: CreateFileMapping fail\n");
        CloseHandle(hFile);
        return;
    }

    mapped_data = MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, mapped_size);
    if (!mapped_data)
    {
        printf("Win32: MapViewOfFile fail\n");
        CloseHandle(hMap);
        CloseHandle(hFile);
        return;
    }

#else
    fd = open(filepath, O_CREAT | O_RDWR, 0666);
    if (fd == -1)
    {
        perror("POSIX: open failed");
        return;
    }

    struct stat st;
    fstat(fd, &st);
    if (st.st_size < (off_t)mapped_size)
    {
        if (ftruncate(fd, mapped_size) == -1)
        {
            perror("ftruncate failed");
            close(fd);
            return;
        }
    }
#endif

    Database *db = (Database *)mapped_data;
    if (db->magic == 0)
    {
        db->magic = SMC_MAGIC;
        db->version = CURRENT_VERSION;
        db->count = 0;
        printf("Inited empty file successfull\n");
    }
    else if (db->magic != SMC_MAGIC)
    {
        printf("Error: Wrong Magic Number (0x%X)\n", db->magic);
        return;
    }

    arena->students = (Student *)((char *)mapped_data + sizeof(Database));
    arena->count = db->count;
}

void db_close_mapping(StudentArena *arena)
{
    if (!mapped_data)
        return;

    Database *db = (Database *)mapped_data;
    db->count = arena->count;

#ifdef _WIN32
    UnmapViewOfFile(mapped_data);
    CloseHandle(hMap);
    CloseHandle(hFile);
    mapped_data = NULL;
    hMap = NULL;
    hFile = INVALID_HANDLE_VALUE;
#else
    msync(mapped_data, mapped_size, MS_SYNC);
    munmap(mapped_data, mapped_size);
    close(fd);
    mapped_data = MAP_FAILED;
    fd = -1;
#endif

    printf("Stop Mapping\n");
}