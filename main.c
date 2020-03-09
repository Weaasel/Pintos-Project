#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include "list.h"
#include "hash.h"
#include "bitmap.h"

#define MAX_NUM 21
#define MAX_LEN 50
#define TOKEN_NUM 6
#define LIST 0
#define HASH 1
#define BITMAP 2
#define NOT_FOUND -1

char buff[251];
char token[TOKEN_NUM][MAX_LEN];

char list_name[MAX_NUM][MAX_LEN];
char hash_name[MAX_NUM][MAX_LEN];
char bitmap_name[MAX_NUM][MAX_LEN];

struct list my_list[MAX_NUM];
struct hash my_hash[MAX_NUM];
struct bitmap *my_bitmap[MAX_NUM];

struct list_item
{
    struct list_elem elem;
    int data;
};
struct hash_item
{
    struct hash_elem elem;
    int data;
};

unsigned hash_hash_int_func (const struct hash_elem *e, void *aux)
{
    struct hash_item *p = hash_entry(e, struct hash_item, elem);
    return hash_int(p->data);
}
bool less_hash_int_func (const struct hash_elem *a, const struct hash_elem *b, void *aux)
{
    struct hash_item *pa = hash_entry(a, struct hash_item, elem);
    struct hash_item *pb = hash_entry(b, struct hash_item, elem);
    if(pa->data < pb->data) return true;
    else return false;
}
bool less_list_int_func (const struct list_elem *a, const struct list_elem *b, void *aux)
{
    struct list_item *pa = list_entry(a, struct list_item, elem);
    struct list_item *pb = list_entry(b, struct list_item, elem);
    if(pa->data < pb->data) return true;
    else return false;
}
void destruct_hash_func (struct hash_elem *e, void *aux)
{
    struct hash_item *p = hash_entry(e, struct hash_item, elem);
    free(p);
}
void square_hash_func (struct hash_elem *e, void *aux)
{
    struct hash_item *p = hash_entry(e, struct hash_item, elem);
    p->data = p->data * p->data;
}
void triple_hash_func (struct hash_elem *e, void *aux)
{
    struct hash_item *p = hash_entry(e, struct hash_item, elem);
    p->data = p->data * p->data * p->data;
}

void list_swap (struct list_elem *a, struct list_elem *b)
{
    struct list_item *pa = list_entry(a, struct list_item, elem);
    struct list_item *pb = list_entry(b, struct list_item, elem);
    int data_tmp = pa->data;
    pa->data = pb->data;
    pb->data = data_tmp;
}
void list_shuffle (struct list *list)
{
    unsigned int i, j, size = list_size(list);
    srand((unsigned)time(NULL));
    for(i = 0; i < size * 10; i++)
    { 
        unsigned idx_a = (unsigned)rand() % size;
        unsigned idx_b = (unsigned)rand() % size;
        struct list_elem *it_a, *it_b;
        for(it_a = list_begin(list), j = 0; it_a != list_end(list) && j < idx_a; it_a = list_next(it_a), j++);
        for(it_b = list_begin(list), j = 0; it_b != list_end(list) && j < idx_b; it_b = list_next(it_b), j++);
        list_swap(it_a, it_b);
    }
}
unsigned hash_int_2 (int i)
{
    unsigned hash = i * 2654435761;
    return hash;
}
struct bitmap *bitmap_expand (struct bitmap *bitmap, int size)
{
    unsigned int i, bitmap_sz = bitmap_size(bitmap);
    struct bitmap *nbitmap = bitmap_create(bitmap_sz + size);
    for(i = 0; i < bitmap_sz; i++)
        bitmap_set(nbitmap, i, bitmap_test(bitmap, i));
    bitmap_destroy(bitmap);
    return nbitmap;
}

// util func
void find_type_idx_by_name (char name[MAX_LEN], int *t, int *idx)
{
    int idx_name;
    idx_name = find_idx_by_name(list_name, name);
    if(idx_name != NOT_FOUND){*t = LIST, *idx = idx_name; return;}
    idx_name = find_idx_by_name(hash_name, name);
    if(idx_name != NOT_FOUND){*t = HASH, *idx = idx_name; return;}
    idx_name = find_idx_by_name(bitmap_name, name);
    if(idx_name != NOT_FOUND){*t = BITMAP, *idx = idx_name; return;}
    ASSERT(idx_name != NOT_FOUND);
}
int find_idx_by_name (char name_list[MAX_NUM][MAX_LEN], char name[MAX_LEN])
{
    int idx_name;
    for(idx_name = 0; idx_name < MAX_NUM; idx_name++)
        if(strcmp(name_list[idx_name], name) == 0) return idx_name;
    return NOT_FOUND;
}
struct list_elem *find_elem_by_idx (struct list *list, int idx)
{
    struct list_elem* it;
    int i;
    for(it = list_begin(list), i = 0; it != list_end(list) && i < idx; it = list_next(it), i++);
    return it;
}
int available_idx (char name_list[MAX_NUM][MAX_LEN])
{
    int i;
    for(i = 0; i < MAX_NUM; i++)
        if(strcmp(name_list[i], "") == 0) return i;
    return 0;
}

int main()
{
    int idx_name;
    for(idx_name = 0; idx_name < MAX_NUM; idx_name++)
    {
        strcpy(list_name[idx_name], "");
        strcpy(hash_name[idx_name], "");
        strcpy(bitmap_name[idx_name], "");
    }

    while(1)
    {
        int token_cnt, aux = 0;
        fgets(buff, sizeof(buff), stdin);
        token_cnt = sscanf(buff, "%s %s %s %s %s %s", token[0], token[1], token[2], token[3], token[4], token[5]);

        if(strcmp(token[0], "create") == 0)
        {
            if(strcmp(token[1], "list") == 0)
            {
                int idx_list = available_idx(list_name);
                strcpy(list_name[idx_list], token[2]);
                list_init(&my_list[idx_list]);
            }
            else if(strcmp(token[1], "hashtable") == 0)
            {
                int idx_hash = available_idx(hash_name);
                strcpy(hash_name[idx_hash], token[2]);
                hash_init(&my_hash[idx_hash], hash_hash_int_func, less_hash_int_func, &aux);
            }
            else if(strcmp(token[1], "bitmap") == 0)
            {
                int idx_bitmap = available_idx(bitmap_name);
                strcpy(bitmap_name[idx_bitmap], token[2]);
                my_bitmap[idx_bitmap] = bitmap_create(atoi(token[3]));
            }
        }
        else if(strcmp(token[0], "delete") == 0)
        {
            int t, idx;
            find_type_idx_by_name(token[1], &t, &idx);

            if(t == LIST)
            {
                while (!list_empty (&my_list[idx]))
                {
                    struct list_elem *e = list_pop_front (&my_list[idx]);
                    struct list_item *p = list_entry(e, struct list_item, elem);
                    free(p);
                }
                strcpy(list_name[idx], "");
            }
            if(t == HASH)
            {
                hash_destroy(&my_hash[idx], destruct_hash_func);
                strcpy(hash_name[idx], "");
            }
            if(t == BITMAP)
            {
                bitmap_destroy(my_bitmap[idx]);
                strcpy(bitmap_name[idx], "");
            }
        }
        else if(strcmp(token[0], "dumpdata") == 0)
        {
            int t, idx;
            find_type_idx_by_name(token[1], &t, &idx);

            if(t == LIST)
            {
                struct list_elem *it;
                for(it = list_begin(&my_list[idx]); it != list_end(&my_list[idx]); it = list_next(it))
                {
                    struct list_item *p = list_entry(it, struct list_item, elem);
                    printf("%d ", p->data);
                }
                printf("\n");
            }
            if(t == HASH)
            {
                struct hash_iterator it;
                hash_first(&it, &my_hash[idx]);
                while(hash_next(&it))
                {
                    struct hash_item *p = hash_entry(hash_cur(&it), struct hash_item, elem);
                    printf("%d ", p->data);
                }
                printf("\n");
            }
            if(t == BITMAP)
            {
                unsigned int i, bitmap_sz = bitmap_size(my_bitmap[idx]);
                for(i = 0; i < bitmap_sz; i++)
                    printf("%d", bitmap_test(my_bitmap[idx], i));
                printf("\n");
            }
        }
        else if(strcmp(token[0], "quit") == 0) break;
        else if(strcmp(token[0], "hash_int_2") == 0)
        {
            int data = atoi(token[1]);
            printf("%d\n", hash_int_2(data));
        }
        else
        {
            int t, idx;
            find_type_idx_by_name(token[1], &t, &idx);

            if(strcmp(token[0], "list_insert") == 0)
            {
                int jdx = atoi(token[2]), data = atoi(token[3]);
                struct list_item *item = (struct list_item*)malloc(sizeof(struct list_item));
                item->data = data;
                struct list_elem *it = find_elem_by_idx(&my_list[idx], jdx);
                list_insert(it, &item->elem);
            }
            else if(strcmp(token[0], "list_splice") == 0)
            {
                int t2, idx2;
                int jdx = atoi(token[2]), idx_a = atoi(token[4]), idx_b = atoi(token[5]);
                find_type_idx_by_name(token[3], &t2, &idx2);
                struct list_elem *it = find_elem_by_idx(&my_list[idx], jdx);
                struct list_elem *it_a = find_elem_by_idx(&my_list[idx2], idx_a);
                struct list_elem *it_b = find_elem_by_idx(&my_list[idx2], idx_b);
                list_splice(it, it_a, it_b);
            }
            else if(strcmp(token[0], "list_push_front") == 0)
            {
                int data = atoi(token[2]);
                struct list_item *item = (struct list_item*)malloc(sizeof(struct list_item));
                item->data = data;
                list_push_front(&my_list[idx], &item->elem);
            }
            else if(strcmp(token[0], "list_push_back") == 0)
            {
                int data = atoi(token[2]);
                struct list_item *item = (struct list_item*)malloc(sizeof(struct list_item));
                item->data = data;
                list_push_back(&my_list[idx], &item->elem);
            }
            else if(strcmp(token[0], "list_remove") == 0)
            {
                int jdx = atoi(token[2]);
                struct list_elem *it = find_elem_by_idx(&my_list[idx], jdx);
                struct list_item *p = list_entry(it, struct list_item, elem);
                list_remove(it);
                free(p);
            }
            else if(strcmp(token[0], "list_pop_front") == 0)
            {
                struct list_elem *it = list_pop_front(&my_list[idx]);
                struct list_item *p = list_entry(it, struct list_item, elem);
                free(p);
            }
            else if(strcmp(token[0], "list_pop_back") == 0)
            {
                struct list_elem *it = list_pop_back(&my_list[idx]);
                struct list_item *p = list_entry(it, struct list_item, elem);
                free(p);
            }
            else if(strcmp(token[0], "list_front") == 0)
            {
                struct list_elem *it = list_front(&my_list[idx]);
                struct list_item *p = list_entry(it, struct list_item, elem);
                printf("%d\n", p->data);
            }
            else if(strcmp(token[0], "list_back") == 0)
            {
                struct list_elem *it = list_back(&my_list[idx]);
                struct list_item *p = list_entry(it, struct list_item, elem);
                printf("%d\n", p->data);
            }
            else if(strcmp(token[0], "list_size") == 0)
            {
                printf("%d\n", list_size(&my_list[idx]));
            }
            else if(strcmp(token[0], "list_empty") == 0)
            {
                if(list_empty(&my_list[idx])) printf("true\n");
                else printf("false\n");
            }
            else if(strcmp(token[0], "list_reverse") == 0)
            {
                list_reverse(&my_list[idx]);
            }
            else if(strcmp(token[0], "list_sort") == 0)
            {
                list_sort(&my_list[idx], less_list_int_func, &aux);
            }
            else if(strcmp(token[0], "list_insert_ordered") == 0)
            {
                int data = atoi(token[2]);
                struct list_item *item = (struct list_item*)malloc(sizeof(struct list_item));
                item->data = data;
                list_insert_ordered(&my_list[idx], &item->elem, less_list_int_func, &aux);
            }
            else if(strcmp(token[0], "list_unique") == 0)
            {
                if(token_cnt == 2)
                {
                    struct list dup;
                    list_init(&dup);
                    list_unique(&my_list[idx], &dup, less_list_int_func, &aux);
                    while (!list_empty (&dup))
                    {
                        struct list_elem *e = list_pop_front (&dup);
                        struct list_item *p = list_entry(e, struct list_item, elem);
                        free(p);
                    }
                }
                else if(token_cnt == 3)
                {
                    int t2, idx2;
                    find_type_idx_by_name(token[2], &t2, &idx2);
                    list_unique(&my_list[idx], &my_list[idx2], less_list_int_func, &aux);
                }
            }
            else if(strcmp(token[0], "list_max") == 0)
            {
                struct list_elem *it = list_max(&my_list[idx], less_list_int_func, &aux);
                struct list_item *p = list_entry(it, struct list_item, elem);
                printf("%d\n", p->data);
            }
            else if(strcmp(token[0], "list_min") == 0)
            {
                struct list_elem *it = list_min(&my_list[idx], less_list_int_func, &aux);
                struct list_item *p = list_entry(it, struct list_item, elem);
                printf("%d\n", p->data);
            }
            else if(strcmp(token[0], "list_swap") == 0)
            {
                int idx_a = atoi(token[2]), idx_b = atoi(token[3]);
                struct list_elem *a = find_elem_by_idx(&my_list[idx], idx_a);
                struct list_elem *b = find_elem_by_idx(&my_list[idx], idx_b);
                list_swap(a, b);
            }
            else if(strcmp(token[0], "list_shuffle") == 0)
            {
                list_shuffle(&my_list[idx]);
            }
            else if(strcmp(token[0], "hash_insert") == 0)
            {
                int data = atoi(token[2]);
                struct hash_item *item = (struct hash_item*)malloc(sizeof(struct hash_item));
                item->data = data;
                hash_insert(&my_hash[idx], &item->elem);
            }
            else if(strcmp(token[0], "hash_replace") == 0)
            {
                int data = atoi(token[2]);
                struct hash_item *item = (struct hash_item*)malloc(sizeof(struct hash_item));
                item->data = data;
                struct hash_elem *it = hash_replace(&my_hash[idx], &item->elem);
                if(it != NULL) free(it);
            }
            else if(strcmp(token[0], "hash_find") == 0)
            {
                struct hash_item item;
                item.data = atoi(token[2]);
                struct hash_elem *it = hash_find(&my_hash[idx], &item.elem);
                if(it != NULL) printf("%d\n", item.data);
            }
            else if(strcmp(token[0], "hash_delete") == 0)
            {
                struct hash_item item;
                item.data = atoi(token[2]);
                struct hash_elem *it = hash_delete(&my_hash[idx], &item.elem);
                if(it != NULL)
                {
                    struct hash_item *p = hash_entry(it, struct hash_item, elem);
                    free(p);
                }
            }
            else if(strcmp(token[0], "hash_clear") == 0)
            {
                hash_clear(&my_hash[idx], destruct_hash_func);
            }
            else if(strcmp(token[0], "hash_size") == 0)
            {
                printf("%d\n", hash_size(&my_hash[idx]));
            }
            else if(strcmp(token[0], "hash_empty") == 0)
            {
                if(hash_empty(&my_hash[idx])) printf("true\n");
                else printf("false\n");
            }
            else if(strcmp(token[0], "hash_apply") == 0)
            {
                if(strcmp(token[2], "square") == 0)
                    hash_apply(&my_hash[idx], square_hash_func);
                else if(strcmp(token[2], "triple") == 0)
                    hash_apply(&my_hash[idx], triple_hash_func);
            }
            else if(strcmp(token[0], "bitmap_size") == 0)
            {
                printf("%d\n", bitmap_size(my_bitmap[idx]));
            }
            else if(strcmp(token[0], "bitmap_set") == 0)
            {
                int jdx = atoi(token[2]);
                if(strcmp(token[3], "true") == 0) bitmap_set(my_bitmap[idx], jdx, true);
                else bitmap_set(my_bitmap[idx], jdx, false);
            }
            else if(strcmp(token[0], "bitmap_mark") == 0)
            {
                int jdx = atoi(token[2]);
                bitmap_mark(my_bitmap[idx], jdx);
            }
            else if(strcmp(token[0], "bitmap_reset") == 0)
            {
                int jdx = atoi(token[2]);
                bitmap_reset(my_bitmap[idx], jdx);
            }
            else if(strcmp(token[0], "bitmap_flip") == 0)
            {
                int jdx = atoi(token[2]);
                bitmap_flip(my_bitmap[idx], jdx);
            }
            else if(strcmp(token[0], "bitmap_test") == 0)
            {
                int jdx = atoi(token[2]);
                if(bitmap_test(my_bitmap[idx], jdx)) printf("true\n");
                else printf("false\n");
            }
            else if(strcmp(token[0], "bitmap_set_all") == 0)
            {
                bool value;
                if(strcmp(token[2], "true") == 0) value = true;
                else value = false;
                bitmap_set_all(my_bitmap[idx], value);
            }
            else if(strcmp(token[0], "bitmap_set_multiple") == 0)
            {
                int start = atoi(token[2]), cnt = atoi(token[3]);
                bool value;
                if(strcmp(token[4], "true") == 0) value = true;
                else value = false;
                bitmap_set_multiple(my_bitmap[idx], start, cnt, value);
            }
            else if(strcmp(token[0], "bitmap_count") == 0)
            {
                int start = atoi(token[2]), cnt = atoi(token[3]);
                bool value;
                if(strcmp(token[4], "true") == 0) value = true;
                else value = false;
                printf("%d\n", bitmap_count(my_bitmap[idx], start, cnt, value));
            }
            else if(strcmp(token[0], "bitmap_contains") == 0)
            {
                int start = atoi(token[2]), cnt = atoi(token[3]);
                bool value;
                if(strcmp(token[4], "true") == 0) value = true;
                else value = false;
                if(bitmap_contains(my_bitmap[idx], start, cnt, value)) printf("true\n");
                else printf("false\n");
            }
            else if(strcmp(token[0], "bitmap_any") == 0)
            {
                int start = atoi(token[2]), cnt = atoi(token[3]);
                if(bitmap_any(my_bitmap[idx], start, cnt)) printf("true\n");
                else printf("false\n");
            }
            else if(strcmp(token[0], "bitmap_none") == 0)
            {
                int start = atoi(token[2]), cnt = atoi(token[3]);
                if(bitmap_none(my_bitmap[idx], start, cnt)) printf("true\n");
                else printf("false\n");
            }
            else if(strcmp(token[0], "bitmap_all") == 0)
            {
                int start = atoi(token[2]), cnt = atoi(token[3]);
                if(bitmap_all(my_bitmap[idx], start, cnt)) printf("true\n");
                else printf("false\n");
            }
            else if(strcmp(token[0], "bitmap_scan") == 0)
            {
                int start = atoi(token[2]), cnt = atoi(token[3]);
                bool value;
                if(strcmp(token[4], "true") == 0) value = true;
                else value = false;
                printf("%u\n", bitmap_scan(my_bitmap[idx], start, cnt, value));
            }
            else if(strcmp(token[0], "bitmap_scan_and_flip") == 0)
            {
                int start = atoi(token[2]), cnt = atoi(token[3]);
                bool value;
                if(strcmp(token[4], "true") == 0) value = true;
                else value = false;
                printf("%u\n", bitmap_scan_and_flip(my_bitmap[idx], start, cnt, value));
            }
            else if(strcmp(token[0], "bitmap_dump") == 0)
            {
                bitmap_dump(my_bitmap[idx]);
            }
            else if(strcmp(token[0], "bitmap_expand") == 0)
            {
                int size = atoi(token[2]);
                my_bitmap[idx] = bitmap_expand(my_bitmap[idx], size);
            }
            else break;
        }
    }
    return 0;
}
