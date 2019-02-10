//
//  hz_splay_tree.h
//  HZAlgorithmKit
//
//  Created by hot on 19/1/19.
//  Copyright © 2019 hot. All rights reserved.
//

// Reference:
// http://www.link.cs.cmu.edu/splay/download/top-down-splay.c

#ifndef hz_splay_tree_h
#define hz_splay_tree_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
    
    typedef struct _hz_splay_node hz_splay_node;
    struct _hz_splay_node {
        uintptr_t item;
        int left;
        int right;
    };

    typedef struct _hz_splay_print_node hz_splay_print_node;
    struct _hz_splay_print_node {
        uintptr_t item;
        int dept;
    };

    typedef struct _hz_splay_tree hz_splay_tree;
    struct _hz_splay_tree {
        hz_splay_node* array;
        int array_len;
        int array_free_index;
        int root_index;       // index of root node in the tree
        int count;            // number of nodes in the tree
        
        hz_splay_print_node* print_array;
        int print_array_len;
        int print_array_count;
        int print_current_dept;
    };
    
    hz_splay_tree* hz_splay_tree_create(void);
    void hz_splay_tree_destroy(hz_splay_tree* p);
    int hz_splay_tree_insert(hz_splay_tree* p, uintptr_t it);
    int hz_splay_tree_delete(hz_splay_tree* p, uintptr_t it);
    hz_splay_node* hz_splay_tree_get(hz_splay_tree* p, int index);
    void hz_splay_tree_print(hz_splay_tree* p);
    
#ifdef DEBUG
    void hz_splay_tree_test(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
