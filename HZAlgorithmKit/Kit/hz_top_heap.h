//
//  hz_top_heap.h
//  HZAlgorithmKit
//
//  Created by hot on 9/2/19.
//  Copyright © 2019年 hot. All rights reserved.
//

#ifndef hz_top_heap_h
#define hz_top_heap_h

#ifdef __cplusplus
extern "C" {
#endif
    
#include <stdio.h>
    
    typedef int (*hz_heap_node_compare_func_type)(void* n1, void* n2);
    typedef void (*hz_heap_node_assign_func_type)(void* dest, void* src);
    
    typedef struct _hz_top_heap hz_top_heap;
    struct _hz_top_heap {
        void* array;
        int top_num;
        int node_size;
        int node_count;
        hz_heap_node_compare_func_type compare_func;
        hz_heap_node_assign_func_type assign_func;
        int has_sort_result;
    };
    
    hz_top_heap* hz_top_heap_create(int topNum, int nodeSize,
                               hz_heap_node_compare_func_type func1,
                               hz_heap_node_assign_func_type func2);
    void hz_top_heap_destroy(hz_top_heap* p);
    void hz_top_heap_insert(hz_top_heap* p, void* newn);
    void* hz_top_heap_build_sort_result(hz_top_heap* p, int* pTopResultNum);

#ifdef __cplusplus
}
#endif
    
#endif
