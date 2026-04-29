#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "student.h"

#define GET_NODE(idx) (&arena->nodes[idx])

StudentArena *arena_init(size_t capacity)
{
    StudentArena *arena = (StudentArena *)malloc(sizeof(StudentArena));
    if (!arena)
        return NULL;

    arena->capacity = capacity;
    return arena;
}

static size_t allocate_node(StudentArena *arena)
{
    return (*arena->next_node)++;
}

static void btree_split_child(StudentArena *arena, size_t parent_idx, int i, size_t child_idx)
{
    BtreeNode *parent = GET_NODE(parent_idx);
    BtreeNode *child = GET_NODE(child_idx);

    size_t new_child_idx = allocate_node(arena);
    BtreeNode *new_child = GET_NODE(new_child_idx);

    new_child->is_leaf = child->is_leaf;
    new_child->num_keys = DEGREE - 1;

    for (int j = 0; j < DEGREE - 1; j++)
    {
        new_child->keys[j] = child->keys[j + DEGREE];
        new_child->student_idx[j] = child->student_idx[j + DEGREE];
    }

    if (!child->is_leaf)
    {
        for (int j = 0; j < DEGREE; j++)
        {
            new_child->children_idx[j] = child->children_idx[j + DEGREE];
        }
    }

    child->num_keys = DEGREE - 1;

    for (int j = parent->num_keys; j >= i + 1; j--)
    {
        parent->children_idx[j + 1] = parent->children_idx[j];
    }

    parent->children_idx[i + 1] = new_child_idx;

    for (int j = parent->num_keys - 1; j >= i; j--)
    {
        parent->keys[j + 1] = parent->keys[j];
        parent->student_idx[j + 1] = parent->student_idx[j];
    }

    parent->keys[i] = child->keys[DEGREE - 1];
    parent->student_idx[i] = child->student_idx[DEGREE - 1];
    parent->num_keys++;
}

static void btree_insert_non_full(StudentArena *arena, size_t node_idx, int key, size_t student_idx)
{
    BtreeNode *node = GET_NODE(node_idx);
    int i = node->num_keys - 1;

    if (node->is_leaf)
    {
        while (i >= 0 && node->keys[i] > key)
        {
            node->keys[i + 1] = node->keys[i];
            node->student_idx[i + 1] = node->student_idx[i];
            i--;
        }
        node->keys[i + 1] = key;
        node->student_idx[i + 1] = student_idx;
        node->num_keys++;
    }
    else
    {
        while (i >= 0 && node->keys[i] > key)
        {
            i--;
        }
        i++;
        if (GET_NODE(node->children_idx[i])->num_keys == 2 * DEGREE - 1)
        {
            btree_split_child(arena, node_idx, i, node->children_idx[i]);
            if (node->keys[i] < key)
                i++;
        }
        btree_insert_non_full(arena, node->children_idx[i], key, student_idx);
    }
}

void arena_add(StudentArena *arena, int id, const char *name, float gpa)
{
    if (*arena->next_student >= arena->capacity || *arena->next_node >= arena->capacity)
    {
        printf("Error: Disk Capacity Full\n");
        return;
    }

    size_t s_idx = *arena->next_student;
    Student *s = &arena->students[s_idx];
    s->id = id;
    strncpy(s->name, name, 31);
    s->name[31] = '\0';
    s->gpa = gpa;
    (*arena->next_student)++;

    if (*arena->root_idx == (size_t)-1)
    {
        *arena->root_idx = allocate_node(arena);
        GET_NODE(*arena->root_idx)->is_leaf = 1;
        GET_NODE(*arena->root_idx)->num_keys = 0;
    }

    BtreeNode *root = GET_NODE(*arena->root_idx);

    if (root->num_keys == 2 * DEGREE - 1)
    {
        size_t new_root_idx = allocate_node(arena);
        BtreeNode *new_root = GET_NODE(new_root_idx);
        new_root->is_leaf = 0;
        new_root->num_keys = 0;
        new_root->children_idx[0] = *arena->root_idx;
        *arena->root_idx = new_root_idx;

        btree_split_child(arena, new_root_idx, 0, new_root->children_idx[0]);
        btree_insert_non_full(arena, new_root_idx, id, s_idx);
    }
    else
    {
        btree_insert_non_full(arena, *arena->root_idx, id, s_idx);
    }

    printf("Inserted to B-Tree\n");
}

static void btree_search(StudentArena *arena, size_t node_idx, int target_id)
{
    if (node_idx == (size_t)-1)
    {
        printf("404 Notfound\n");
        return;
    }

    BtreeNode *node = GET_NODE(node_idx);
    int i = 0;

    while (i < node->num_keys && target_id > node->keys[i])
        i++;

    if (i < node->num_keys && target_id == node->keys[i])
    {
        Student *s = &arena->students[node->student_idx[i]];
        printf("FOUND: ID %d Name %-15s GPA %.1f\n", s->id, s->name, s->gpa);
        return;
    }
    if (node->is_leaf)
    {
        printf("404 Notfound\n");
    }

    btree_search(arena, node->children_idx[i], target_id);
}

void arena_search_id(StudentArena *arena, int target_id)
{
    btree_search(arena, *arena->root_idx, target_id);
}

static void btree_transverse(const StudentArena *arena, size_t node_idx)
{
    if (node_idx == (size_t)-1)
        return;
    BtreeNode *node = GET_NODE(node_idx);
    int i;
    for (i = 0; i < node->num_keys; i++)
    {
        if (!node->is_leaf)
            btree_transverse(arena, node->children_idx[i]);
        Student *s = &arena->students[node->student_idx[i]];
        printf("ID: %d | Name: %-15s | GPA: %.1f\n", s->id, s->name, s->gpa);
    }
    if (!node->is_leaf)
        btree_transverse(arena, node->children_idx[i]);
}

void arena_dump(const StudentArena *arena)
{
    if (*arena->root_idx == (size_t)-1)
    {
        printf("Arena Empty\n");
        return;
    }

    btree_transverse(arena, *arena->root_idx);
}

void arena_free(StudentArena *arena)
{
    if (arena)
    {
        free(arena);
    }
}
