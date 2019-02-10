//
//  hz_splay_tree.c
//  HZAlgorithmKit
//
//  Created by hot on 19/1/19.
//  Copyright © 2019 hot. All rights reserved.
//

#include "hz_splay_tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

hz_splay_tree* hz_splay_tree_create(int initLen, int incLen, int nodeSize, hz_splay_node_compare_func_type compareFunc) {
    hz_splay_tree* p = calloc(1, sizeof(hz_splay_tree));
    if(!p) {
        return 0;
    }
    p->array = calloc(initLen, nodeSize);
    if(!p->array) {
        free(p);
        return 0;
    }
    p->array_init_len = initLen;
    p->array_inc_len = incLen;
    p->array_len = initLen;
    p->array_free_index = 0;
    p->root_index = -1;
    p->node_size = nodeSize;
    p->node_count = 0;
    p->compareFunc = compareFunc;
    
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

#define _NODE_INDEX(arr,pos) (void*)(((char*)(arr))+nodeSize*(pos))

// Simple top down splay, not requiring item to be in the tree tindex.
int hz_splay_tree_splay(hz_splay_tree* p, hz_splay_node* it, int tindex ) {
    if(!p) return -1;
    if(!p->array) return -1;
    if (tindex == -1) return -1;
    
    hz_splay_node N, *l, *r, *y;
    N.left = N.right = -1;
    l = r = &N;
    int yindex;
    
    int nodeSize = p->node_size;
    hz_splay_node_compare_func_type compareFunc = p->compareFunc;
    
    hz_splay_node* arr = p->array;
    hz_splay_node* t = _NODE_INDEX(arr, tindex);
    
    for (;;) {
        int compareRes = compareFunc(it, t);
        if (compareRes > 0) {
            if (t->left == -1) break;
            yindex = t->left;
            y = _NODE_INDEX(arr, yindex);
            if (compareFunc(it, y) > 0) { // rotate right
                t->left = y->right;
                y->right = tindex;
                tindex = yindex;
                t = y;
                if (t->left == -1) break;
            }
            r->left = tindex; // link right
            r = t;
            tindex = t->left;
            t = _NODE_INDEX(arr, tindex);
        } else if (compareRes < 0) {
            if (t->right == -1) break;
            yindex = t->right;
            y = _NODE_INDEX(arr, yindex);
            if (compareFunc(it, y) < 0) { // rotate left
                t->right = y->left;
                y->left = tindex;
                tindex = yindex;
                t = y;
                if (t->right == -1) break;
            }
            l->right = tindex; // link left
            l = t;
            tindex = t->right;
            t = _NODE_INDEX(arr, tindex);
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

int hz_splay_tree_insert(hz_splay_tree* p, hz_splay_node* it, int* pIsInserted) {
    if(!p) return -1;
    if(!p->array) return -1;
    
    *pIsInserted = 0;
    
    int nodeSize = p->node_size;
    
    if(p->node_count == p->array_len) {
        int newlen = p->array_len + p->array_inc_len;
        hz_splay_node* newarray = realloc(p->array, nodeSize * newlen);
        if(!newarray) {
            return -1;
        }
        memset(newarray+p->array_len, 0, nodeSize * p->array_inc_len);
        p->array = newarray;
        p->array_len = newlen;
    }

    hz_splay_node* arr = p->array;
    
    int newindex = p->array_free_index;
    hz_splay_node* ne = _NODE_INDEX(arr, newindex);
    if(ne->left > 0) {
        p->array_free_index = ne->left - 1;
    } else {
        p->array_free_index ++;
    }

    ne->left = -1;
    ne->right = -1;
    
    int tindex = p->root_index;
    if(tindex == -1) {
        p->node_count = 1;
        p->root_index = newindex;
        *pIsInserted = 1;
        return newindex;
    }
    
    tindex = hz_splay_tree_splay(p, it, tindex);
    p->root_index = tindex;
    
    hz_splay_node_compare_func_type compareFunc = p->compareFunc;
    
    hz_splay_node* t = _NODE_INDEX(arr, tindex);
    int compareRes = compareFunc(it, t);
    if (compareRes > 0) {
        ne->left = t->left;
        ne->right = tindex;
        t->left = -1;
        
        p->node_count++;
        p->root_index = newindex;
        *pIsInserted = 1;
        return newindex;
    } else if (compareRes < 0) {
        ne->right = t->right;
        ne->left = tindex;
        t->right = -1;
        
        p->node_count++;
        p->root_index = newindex;
        *pIsInserted = 1;
        return newindex;
    } else { // We get here if it's already in the tree, Don't add it again.
        ne->left = p->array_free_index + 1;
        p->array_free_index = newindex;
        
        return tindex;
    }
    return -1;
}

// Deletes i from the tree if it's there, Return a pointer to the resulting tree.
int hz_splay_tree_delete(hz_splay_tree* p, hz_splay_node* it) {
    if(!p) return -1;
    if(!p->array) return -1;
    
    int tindex = p->root_index;
    if(tindex == -1) return -1;
    
    int x;

    tindex = hz_splay_tree_splay(p, it, tindex);
    p->root_index = tindex;
    
    int nodeSize = p->node_size;
    hz_splay_node_compare_func_type compareFunc = p->compareFunc;
    
    hz_splay_node* arr = p->array;
    hz_splay_node* t = _NODE_INDEX(arr, tindex);
    
    if (compareFunc(it, t) == 0) {
        if (t->left == -1) {
            x = t->right;
        } else {
            x = hz_splay_tree_splay(p, it, t->left);
            hz_splay_node* xn = _NODE_INDEX(arr, x);
            xn->right = t->right;
        }
        p->node_count--;
        
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
    
    int nodeSize = p->node_size;
    
    return _NODE_INDEX(p->array, index);
}

void hz_splay_tree_traverse_in(hz_splay_tree* p, hz_splay_node_visit_func_type visitFunc, int tindex) {
    if(!p) return;
    if(!p->array) return;
    if(tindex == -1) return;
    
    int nodeSize = p->node_size;
    
    hz_splay_node* arr = p->array;
    hz_splay_node* t = _NODE_INDEX(arr, tindex);
    int l = t->left;
    int r = t->right;
    
    ++p->current_traverse_dept;
    hz_splay_tree_traverse_in(p, visitFunc, l);
    
    visitFunc(p, t, p->current_traverse_dept);
    
    hz_splay_tree_traverse_in(p, visitFunc, r);
    --p->current_traverse_dept;
}

void hz_splay_tree_traverse(hz_splay_tree* p, hz_splay_node_visit_func_type visitFunc) {
    if(!p) return;
    if(!p->array) return;

    p->current_traverse_dept = 0;
    hz_splay_tree_traverse_in(p, visitFunc, p->root_index);
    p->current_traverse_dept = 0;
}
