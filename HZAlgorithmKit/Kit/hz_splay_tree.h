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
        int left;
        int right;
    };
    
    typedef int (*hz_splay_node_compare_func_type)(hz_splay_node* n1, hz_splay_node* n2);

    typedef struct _hz_splay_tree hz_splay_tree;
    struct _hz_splay_tree {
        hz_splay_node* array;
        int array_init_len;
        int array_inc_len;
        int array_len;
        int array_free_index;
        int root_index;       // index of root node in the tree
        int node_count;       // number of nodes in the tree
        int node_size;
        hz_splay_node_compare_func_type compareFunc;
        int current_traverse_dept;
    };

    typedef void (*hz_splay_node_visit_func_type)(hz_splay_tree* p, hz_splay_node* n, int dept);
    
    hz_splay_tree* hz_splay_tree_create(int initLen, int incLen, int nodeSize, hz_splay_node_compare_func_type compareFunc);
    void hz_splay_tree_destroy(hz_splay_tree* p);
    int hz_splay_tree_insert(hz_splay_tree* p, hz_splay_node* it, int* pIsInserted);
    int hz_splay_tree_delete(hz_splay_tree* p, hz_splay_node* it);
    hz_splay_node* hz_splay_tree_get(hz_splay_tree* p, int index);
    void hz_splay_tree_traverse(hz_splay_tree* p, hz_splay_node_visit_func_type visitFunc);

#ifdef __cplusplus
}
#endif

#endif
