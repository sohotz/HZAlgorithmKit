//
//  hz_stack_table.c
//  HZAlgorithmKit
//
//  Created by hot on 19/1/19.
//  Copyright © 2019 hot. All rights reserved.
//

#include "hz_stack_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mach/vm_statistics.h>
#include <mach-o/dyld.h>
#include <dlfcn.h>
#include <sys/sysctl.h>

#define _ARRAY_INIT_LEN 1024*1024*2

hz_stack_table* hz_stack_table_create() {
    hz_stack_table* p = calloc(1, sizeof(hz_stack_table));
    if(!p) return 0;
    
    p->array = calloc(_ARRAY_INIT_LEN, sizeof(hz_addr_node));
    if(!p->array) {
        free(p);
        return 0;
    }
    
    p->array_len = _ARRAY_INIT_LEN;
    
    return p;
}

void hz_stack_table_destroy(hz_stack_table* p) {
    if(p) {
        if(p->array) {
            free(p->array);
        }
        free(p);
    }
}

// return stack entry index
int hz_stack_table_insert(hz_stack_table* p, void** addrlist, int cnt) {
    if(!addrlist) return -1;
    if(cnt <= 0) return -1;
    
    static uintptr_t imageAddress = 0;
    if(imageAddress == 0) {
        const struct mach_header* header = _dyld_get_image_header(0);
        imageAddress = (uintptr_t)header;
    }

    int lastParent = -1;
    uintptr_t hashLastAddr = 0;
    
    for(int i = cnt-1; i >= 0; i--) {
        
        void* addr = addrlist[i];
        
        uintptr_t hashAddr = (uintptr_t)(addr) - imageAddress;
        
        int hash = (uintptr_t)(((uintptr_t)(hashAddr)&0x3ffff<<1)
                               + ((uintptr_t)(hashAddr)>>18<<3)
                               + ((uintptr_t)(hashLastAddr)&0x3ffff<<1)
                               + ((uintptr_t)(hashLastAddr)>>18<<3))%(0x200000);
        hashLastAddr += hashAddr;
        
        hz_addr_node* h = p->array + hash;
        uintptr_t newValue = ((uintptr_t)(lastParent+1) << 36) | ((uintptr_t)addr & 0xfffffffff);
        if(h->value == 0) {
            h->value = newValue;
            lastParent = hash;
            ++p->hit1;
        } else if(h->value == newValue) {
            lastParent = hash;
            ++p->hit2;
        } else {
            int rawk, k;
            int limit = 32;//p->array_len;
            for(rawk = hash+1; rawk < hash+limit; rawk+=1) {
                if((rawk-hash)%4<=0) continue;
                k = rawk % p->array_len;
                hz_addr_node* kn = p->array + k;
                if(kn->value == 0) {
                    kn->value = newValue;
                    lastParent = k;
                    ++p->hit3;
                    break;
                } else if(kn->value == newValue) {
                    lastParent = k;
                    ++p->hit4;
                    break;
                }
            }
            if(rawk >= hash+limit) {
                ++p->hit5;
            }
        }
    }
    
    return lastParent;
}

int hz_stack_table_get(hz_stack_table* p, int stackindex, void** addrlist, int* cnt) {
    if(!addrlist || !cnt) return -1;
    
    int len = *cnt;
    if(len <= 0) return -1;
    
    int i = 0;
    hz_addr_node* n = p->array + stackindex;
    addrlist[i++] = (void*)(n->value & 0xfffffffff);
    int parent = (n->value >> 36) - 1;
    while (parent != -1 && i < len) {
        n = p->array + parent;
        addrlist[i++] = (void*)(n->value & 0xfffffffff);
        parent = (n->value >> 36) - 1;
    }
    *cnt = i;
    return i;
}
