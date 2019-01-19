//
//  hz_splay_tree.c
//  HZAlgorithmKit
//
//  Created by hot on 19/1/19.
//  Copyright © 2019年 hot. All rights reserved.
//

#include "hz_splay_tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define _ARRAY_INIT_LEN 1024*256
#define _ARRAY_INC_LEN  1024*64

hz_splay_tree* hz_splay_tree_create() {
    hz_splay_tree* p = calloc(1, sizeof(hz_splay_tree));
    if(!p) {
        return 0;
    }
    p->array = calloc(_ARRAY_INIT_LEN, sizeof(hz_splay_node));
    if(!p->array) {
        free(p);
        return 0;
    }
    p->array_len = _ARRAY_INIT_LEN;
    p->array_free_index = 0;
    p->root_index = -1;
    p->count = 0;
    
    return p;
}

void hz_splay_tree_destroy(hz_splay_tree* p) {
    if(p) {
        if(p->array) {
            free(p->array);
        }
        free(p);
    }
}

// Simple top down splay, not requiring item to be in the tree tindex.
int hz_splay_tree_splay(hz_splay_tree* p, uintptr_t it, int tindex ) {
    if(!p) return -1;
    if(!p->array) return -1;
    if (tindex == -1) return -1;
    
    hz_splay_node N, *l, *r, *y;
    N.left = N.right = -1;
    l = r = &N;
    int yindex;
    
    hz_splay_node* arr = p->array;
    hz_splay_node* t = arr + tindex;
    
    for (;;) {
        if (it < t->item) {
            if (t->left == -1) break;
            yindex = t->left;
            y = arr + yindex;
            if (it < y->item) { // rotate right
                t->left = y->right;
                y->right = tindex;
                tindex = yindex;
                t = y;
                if (t->left == -1) break;
            }
            r->left = tindex; // link right
            r = t;
            tindex = t->left;
            t = arr + tindex;
        } else if (it > t->item) {
            if (t->right == -1) break;
            yindex = t->right;
            y = arr + yindex;
            if (it > y->item) { // rotate left
                t->right = y->left;
                y->left = tindex;
                tindex = yindex;
                t = y;
                if (t->right == -1) break;
            }
            l->right = tindex; // link left
            l = t;
            tindex = t->right;
            t = arr + tindex;
        } else {
            break;
        }
    }
    l->right = t->left; // assemble
    r->left = t->right;
    t->left = N.right;
    t->right = N.left;
    return tindex;
}

int hz_splay_tree_insert(hz_splay_tree* p, uintptr_t it) {
    if(!p) return -1;
    if(!p->array) return -1;
    
    if(p->count == p->array_len) {
        int newlen = p->array_len + _ARRAY_INC_LEN;
        hz_splay_node* newarray = realloc(p->array, sizeof(hz_splay_node) * newlen);
        if(!newarray) {
            return -1;
        }
        memset(newarray+p->array_len, 0, sizeof(hz_splay_node) * _ARRAY_INC_LEN);
        p->array = newarray;
        p->array_len = newlen;
    }

    hz_splay_node* arr = p->array;
    
    int newindex = p->array_free_index;
    hz_splay_node* ne = arr + newindex;
    if(ne->left > 0) {
        p->array_free_index = ne->left - 1;
    } else {
        p->array_free_index ++;
    }

    ne->left = -1;
    ne->right = -1;
    ne->item = it;
    
    int tindex = p->root_index;
    if(tindex == -1) {
        p->count = 1;
        p->root_index = newindex;
        return newindex;
    }
    
    tindex = hz_splay_tree_splay(p, it, tindex);
    p->root_index = tindex;
    
    hz_splay_node* t = arr + tindex;
    if (it < t->item) {
        ne->left = t->left;
        ne->right = tindex;
        t->left = -1;
        
        p->count++;
        p->root_index = newindex;
        return newindex;
    } else if (it > t->item) {
        ne->right = t->right;
        ne->left = tindex;
        t->right = -1;
        
        p->count++;
        p->root_index = newindex;
        return newindex;
    } else { // We get here if it's already in the tree, Don't add it again.
        ne->left = p->array_free_index + 1;
        p->array_free_index = newindex;
        
        return tindex;
    }
    return -1;
}

// Deletes i from the tree if it's there, Return a pointer to the resulting tree.
int hz_splay_tree_delete(hz_splay_tree* p, uintptr_t it) {
    if(!p) return -1;
    if(!p->array) return -1;
    
    int tindex = p->root_index;
    if(tindex == -1) return -1;
    
    int x;

    tindex = hz_splay_tree_splay(p, it, tindex);
    p->root_index = tindex;
    
    hz_splay_node* arr = p->array;
    hz_splay_node* t = arr + tindex;
    
    if (it == t->item) {
        if (t->left == -1) {
            x = t->right;
        } else {
            x = hz_splay_tree_splay(p, it, t->left);
            arr[x].right = t->right;
        }
        p->count--;
        
        t->left = p->array_free_index + 1; // index 1 2 3...
        p->array_free_index = tindex;
        
        p->root_index = x;
        return x;
    }
    return tindex;
}

hz_splay_node* hz_splay_tree_get(hz_splay_tree* p, int index) {
    if(!p) return 0;
    if(!p->array) return 0;
    if(index < 0 || index >= p->array_len) return 0;
    
    return &p->array[index];
}

void hz_splay_tree_print_inner(hz_splay_tree* p, int tindex) {
    if(!p) return;
    if(!p->array) return;
    if(tindex == -1) return;
    
    hz_splay_node* arr = p->array;
    hz_splay_node* t = arr + tindex;
    int l = t->left;
    int r = t->right;
    
    ++p->print_current_dept;
    
    hz_splay_tree_print_inner(p, l);
    
    if(p->print_array_count == p->print_array_len) {
        int newlen = p->print_array_len + _ARRAY_INC_LEN;
        hz_splay_print_node* newarray = realloc(p->print_array, sizeof(hz_splay_print_node) * newlen);
        if(!newarray) return;
        p->print_array = newarray;
        p->print_array_len = newlen;
    }
    
    p->print_array[p->print_array_count].item = t->item;
    p->print_array[p->print_array_count++].dept = p->print_current_dept;
    
    hz_splay_tree_print_inner(p, r);
    
    --p->print_current_dept;
}

void hz_splay_tree_print(hz_splay_tree* p) {
    if(!p) return;
    if(!p->array) return;
    
    p->print_array = calloc(_ARRAY_INIT_LEN, sizeof(hz_splay_print_node));
    if(!p->print_array) return;
    
    p->print_array_len = _ARRAY_INIT_LEN;
    p->print_array_count = 0;
    p->print_current_dept = 0;
    
    hz_splay_tree_print_inner(p, p->root_index);
    
    printf("splay tree(%p,count=%d):\n", p, p->count);
    for(int i = 0; i < p->print_array_count; i++) {
        printf("[%d]%ld(%d)\n", i, (long)p->print_array[i].item, p->print_array[i].dept);
        if(i < p->print_array_count-1 && p->print_array[i].item >= p->print_array[i+1].item) {
            printf("error - [%d]%ld(%d) >= [%d]%ld(%d)\n",
                   i, (long)p->print_array[i].item, p->print_array[i].dept,
                   i+1, (long)p->print_array[i+1].item, p->print_array[i+1].dept);
        }
    }
    
    free(p->print_array);
    p->print_array_len = 0;
    p->print_array_count = 0;
    p->print_current_dept = 0;
}

#ifdef DEBUG
void hz_splay_tree_test() {
    hz_splay_tree* p = hz_splay_tree_create();
    
    int i;
    for(i = 0; i < 100000; i++) {
        hz_splay_tree_insert(p, rand()%100000);
    }
    hz_splay_tree_print(p);
    
    for(i = 0; i < 100000; i++) {
        hz_splay_tree_delete(p, rand()%100000);
    }
    hz_splay_tree_print(p);
    
    hz_splay_tree_destroy(p);
    p = 0;
}
#endif
