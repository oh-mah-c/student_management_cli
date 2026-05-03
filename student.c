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

static int find_key(BtreeNode *node, int k)
{
    int idx = 0;
    while (idx < node->num_keys && node->keys[idx] < k)
        ++idx;
    return idx;
}

static int get_pred(StudentArena *arena, size_t node_idx)
{
    BtreeNode *curr = GET_NODE(node_idx);
    while (!curr->is_leaf)
    {
        curr = GET_NODE(curr->children_idx[curr->num_keys]);
    }

    return curr->keys[curr->num_keys - 1];
}

static int get_successor(StudentArena *arena, size_t node_idx)
{
    BtreeNode *curr = GET_NODE(node_idx);
    while (!curr->is_leaf)
    {
        curr = GET_NODE(curr->children_idx[0]);
    }
    return curr->keys[0];
}

static void merge_nodes(StudentArena *arena, size_t node_idx, int idx)
{
    BtreeNode *node = GET_NODE(node_idx);
    size_t child_idx = node->children_idx[idx];
    size_t sibling_idx = node->children_idx[idx + 1];
    BtreeNode *child = GET_NODE(child_idx);
    BtreeNode *sibling = GET_NODE(sibling_idx);

    child->keys[DEGREE - 1] = node->keys[idx];
    child->children_idx[DEGREE - 1] = node->student_idx[idx];

    for (int i = 0; i < sibling->num_keys; ++i)
    {
        child->keys[i + DEGREE] = sibling->keys[i];
        child->student_idx[i + DEGREE] = sibling->student_idx[i];
    }

    if (!child->is_leaf)
    {
        for (int i = 0; i <= sibling->num_keys; ++i)
        {
            child->children_idx[i + DEGREE] = sibling->children_idx[i];
        }
    }

    child->num_keys = sibling->num_keys + 1;

    for (int i = idx + 1; i < node->num_keys; ++i)
    {
        node->keys[i - 1] = node->keys[i];
        node->student_idx[i - 1] = node->student_idx[i];
    }

    for (int i = idx + 2; i <= node->num_keys; ++i)
    {
        node->children_idx[i - 1] = node->children_idx[i];
    }

    node->num_keys--;
}

static void fill_node(StudentArena *arena, size_t node_idx, int idx)
{
    BtreeNode *node = GET_NODE(node_idx);

    if (idx != 0 && GET_NODE(node->children_idx[idx - 1])->num_keys >= DEGREE)
    {
        BtreeNode *child = GET_NODE(node->children_idx[idx]);
        BtreeNode *sibling = GET_NODE(node->children_idx[idx - 1]);

        for (int i = child->num_keys - 1; i >= 0; --i)
        {
            child->keys[i + 1] = child->keys[i];
            child->student_idx[i + 1] = child->student_idx[idx - 1];
        }

        if (!child->is_leaf)
        {
            for (int i = child->num_keys; i >= 0; --i)
            {
                child->children_idx[i + 1] = child->children_idx[i];
            }
            child->children_idx[0] = sibling->children_idx[sibling->num_keys];
        }

        child->keys[0] = node->keys[idx - 1];
        child->student_idx[0] = node->student_idx[idx - 1];
        node->keys[idx - 1] = sibling->keys[sibling->num_keys - 1];
        node->student_idx[idx - 1] = sibling->student_idx[sibling->num_keys - 1];
        child->num_keys++;
        sibling->num_keys--;
    }
    else if (idx != node->num_keys && GET_NODE(node->children_idx[idx + 1])->num_keys >= DEGREE)
    {
        BtreeNode *child = GET_NODE(node->children_idx[idx]);
        BtreeNode *sibling = GET_NODE(node->children_idx[idx + 1]);

        child->keys[idx] = sibling->keys[0];
        node->student_idx[idx] = sibling->student_idx[0];

        for (int i = 1; i < sibling->num_keys; ++i)
        {
            sibling->keys[i - 1] = sibling->keys[i];
            sibling->student_idx[i - 1] = sibling->student_idx[i];
        }
        if (!sibling->is_leaf)
        {
            for (int i = 1; i <= sibling->num_keys; ++i)
            {
                sibling->children_idx[i - 1] = sibling->children_idx[i];
            }
        }

        child->num_keys++;
        sibling->num_keys--;
    }
    else
    {
        if (idx != node->num_keys)
        {
            merge_nodes(arena, node_idx, idx);
        }
        else
        {
            merge_nodes(arena, node_idx, idx - 1);
        }
    }
}

static void btree_delete_internal(StudentArena *arena, size_t node_idx, int k)
{
    if (node_idx == (size_t)-1)
        return;
    BtreeNode *node = GET_NODE(node_idx);
    int idx = find_key(node, k);

    if (idx < node->num_keys && node->keys[idx] == k)
    {
        if (node->is_leaf)
        {
            for (int i = idx + 1; i < node->num_keys; ++i)
            {
                node->keys[i - 1] = node->keys[i];
                node->student_idx[i - 1] = node->student_idx[i];
            }
            node->num_keys--;
        }
        else
        {
            size_t left_child = node->children_idx[idx];
            size_t right_child = node->children_idx[idx + 1];

            if (GET_NODE(left_child)->num_keys >= DEGREE)
            {
                int pred = get_pred(arena, left_child);
                node->keys[idx] = pred;
                btree_delete_internal(arena, left_child, pred);
            }
            else if (GET_NODE(right_child)->num_keys >= DEGREE)
            {
                int succ = get_successor(arena, right_child);
                node->keys[idx] = succ;
                btree_delete_internal(arena, right_child, succ);
            }
            else
            {
                merge_nodes(arena, node_idx, idx);
                btree_delete_internal(arena, left_child, k);
            }
        }
    }
    else
    {
        if (node->is_leaf)
        {
            printf("404: Can not delete ID %d do not existed\n", k);
            return;
        }

        int flag = (idx == node->num_keys);

        if (GET_NODE(node->children_idx[idx])->num_keys < DEGREE)
        {
            fill_node(arena, node_idx, idx);
        }

        if (flag && idx > node->num_keys)
        {
            btree_delete_internal(arena, node->children_idx[idx - 1], k);
        }
        else
        {
            btree_delete_internal(arena, node->children_idx[idx], k);
        }
    }
}

void arena_delete_id(StudentArena *arena, int target_id)
{
    if (*arena->root_idx == (size_t)-1)
    {
        printf("BTree empty\n");
        return;
    }

    btree_delete_internal(arena, *arena->root_idx, target_id);

    BtreeNode *root = GET_NODE(*arena->root_idx);
    if (root->num_keys == 0)
    {
        if (root->is_leaf)
        {
            *arena->root_idx = (size_t)-1;
        }
        else
        {
            *arena->root_idx = root->children_idx[0];
        }
    }
    printf("Delete student with ID: %d", target_id);
}

static Student *btree_get_student(StudentArena *arena, size_t node_idx, int target_id)
{
    if (node_idx == (size_t)-1)
        return NULL;

    BtreeNode *node = GET_NODE(node_idx);
    int i = 0;

    while (i < node->num_keys && target_id > node->keys[i])
    {
        i++;
    }

    if (i < node->num_keys && target_id == node->keys[i])
    {
        return &arena->students[node->student_idx[i]];
    }

    if (node->is_leaf)
    {
        return NULL;
    }

    return btree_get_student(arena, node->student_idx[i], target_id);
}

void arena_update_student(StudentArena *arena, int target_id, const char *new_name, float new_gpa)
{
    if (*arena->root_idx == (size_t)-1)
    {
        printf("BTree Empty\n");
        return;
    }

    Student *s = btree_get_student(arena, *arena->root_idx, target_id);

    if (s)
    {
        strncpy(s->name, new_name, 31);
        s->name[31] = '\0';
        s->gpa = new_gpa;
        printf("Updated successfully [%p] ID: %d | Name: %-15s | GPA: %1.f\n", (void *)s, s->id, s->name, s->gpa);
    }
    else
    {
        printf("Cannot Update. Can not find ID\n");
    }
}