//
//  hz_stack_table.h
//  HZAlgorithmKit
//
//  Created by hot on 19/1/19.
//  Copyright © 2019年 hot. All rights reserved.
//

#ifndef hz_stack_table_h
#define hz_stack_table_h

#ifdef __cplusplus
extern "C" {
#endif
    
#include <stdio.h>
    
    typedef struct _hz_addr_node hz_addr_node;
    struct _hz_addr_node {
        union {
            uintptr_t value;
            struct {
                uintptr_t parent:28;
                uintptr_t item:36;
            };
        };
    };
    
    typedef struct _hz_stack_table hz_stack_table;
    struct _hz_stack_table {
        hz_addr_node* array;
        int array_len;
        int hit1, hit2, hit3, hit4, hit5;
    };
    
    hz_stack_table* hz_stack_table_create(void);
    void hz_stack_table_destroy(hz_stack_table* p);
    int hz_stack_table_insert(hz_stack_table* p, void** addrlist, int cnt);
    int hz_stack_table_get(hz_stack_table* p, int stackindex, void** addrlist, int* cnt);

#ifdef DEBUG
    void hz_stack_table_test(void);
#endif
    
#ifdef __cplusplus
}
#endif

#endif
