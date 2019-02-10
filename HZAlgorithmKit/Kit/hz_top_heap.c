//
//  hz_top_heap.c
//  HZAlgorithmKit
//
//  Created by hot on 9/2/19.
//  Copyright © 2019年 hot. All rights reserved.
//

#include "hz_top_heap.h"
#include <stdio.h>
#include <stdlib.h>

hz_top_heap* hz_top_heap_create(int topNum, int nodeSize,
                                hz_heap_node_compare_func_type func1,
                                hz_heap_node_assign_func_type func2) {
    hz_top_heap* p = calloc(1, sizeof(hz_top_heap));
    if(!p) return 0;
    
    p->top_num = topNum;
    
    p->array = calloc((p->top_num + 1), nodeSize);
    if(!p->array) {
        free(p);
        return 0;
    }
    
    p->node_size = nodeSize;
    p->node_count = 0;
    p->compare_func = func1;
    p->assign_func = func2;
    return p;
}

void hz_top_heap_destroy(hz_top_heap* p) {
    if(p) {
        if(p->array) {
            free(p->array);
        }
        free(p);
    }
}

#define _NODE_INDEX(arr,pos) (void*)(((char*)(arr))+nodeSize*(pos))

void hz_top_heap_min_heapify(hz_top_heap* p, void* tmpn, int arrBegin, int start, int end) {
    int nodeSize = p->node_size;
    void* arr = _NODE_INDEX(p->array, arrBegin);
    hz_heap_node_compare_func_type compareFunc = p->compare_func;
    hz_heap_node_assign_func_type assignFunc = p->assign_func;
    
    int dad = start;
    int son = dad * 2 + 1;
    while (son <= end) {
        if(son + 1 <= end && compareFunc(_NODE_INDEX(arr, son), _NODE_INDEX(arr, son+1)) > 0)
            son++;
        if(compareFunc(_NODE_INDEX(arr, dad), _NODE_INDEX(arr, son)) < 0)
            return;
        else {
            void* dadN = _NODE_INDEX(arr, dad);
            void* sonN = _NODE_INDEX(arr, son);
            assignFunc(tmpn, dadN);
            assignFunc(dadN, sonN);
            assignFunc(sonN, tmpn);
            dad = son;
            son = dad * 2 + 1;
        }
    }
}

void hz_top_heap_insert(hz_top_heap* p, void* newn) {
    if(!p) return;
    if(!p->array) return;
    if(p->has_sort_result) return;
    
    void* arr = p->array;
    int nodeSize = p->node_size;
    hz_heap_node_assign_func_type assignFunc = p->assign_func;
    
    if(p->node_count < p->top_num + 1) {
        int pos = p->node_count;
        void* dest = _NODE_INDEX(arr, pos);
        assignFunc(dest, newn);
        ++p->node_count;
        
        if(p->node_count == p->top_num + 1) {
            int len = p->top_num + 1;
            for (int i = len / 2 - 1; i >= 0; i--) {
                hz_top_heap_min_heapify(p, newn, 0, i, len - 1);
            }
        }
    } else {
        assignFunc(arr, newn);
        int len = p->top_num + 1;
        hz_top_heap_min_heapify(p, newn, 0, 0, len -1);
    }
}

void* hz_top_heap_build_sort_result(hz_top_heap* p, int* pTopResultNum) {
    if(!p) return 0;
    if(!p->array) return 0;
    if(p->node_count <= 0) return 0;
    
    int nodeSize = p->node_size;
    hz_heap_node_assign_func_type assignFunc = p->assign_func;
    
    void* tmpn = 0;
    int arrBegin = 0;
    int len = 0;
    
    if(p->node_count >= p->top_num + 1) {
        
        tmpn = p->array;
        arrBegin = 1;
        len = p->top_num;
        
        *pTopResultNum = p->top_num;
    } else {
        
        tmpn = _NODE_INDEX(p->array, p->top_num);
        arrBegin = 0;
        len = p->node_count;
        
        *pTopResultNum = p->node_count;
    }
    
    void* arr = _NODE_INDEX(p->array, arrBegin);
    int i = 0;
    for(i = len / 2 - 1; i >= 0; i--) {
        hz_top_heap_min_heapify(p, tmpn, arrBegin, i, len - 1);
    }
    for(i = len - 1; i > 0; i--) {
        void* firstN = arr;
        void* sortN = _NODE_INDEX(arr, i);
        assignFunc(tmpn, firstN);
        assignFunc(firstN, sortN);
        assignFunc(sortN, tmpn);
        hz_top_heap_min_heapify(p, tmpn, arrBegin, 0, i - 1);
    }
    
    p->has_sort_result = 1;
    
    return arr;
}
