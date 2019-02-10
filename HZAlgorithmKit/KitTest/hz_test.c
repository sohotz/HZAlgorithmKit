//
//  hz_test.c
//  HZAlgorithmKit
//
//  Created by hot on 10/2/19.
//  Copyright © 2019 hot. All rights reserved.
//

#include "hz_test.h"
#include "hz_splay_tree.h"
#include "hz_stack_table.h"
#include "hz_top_heap.h"
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// ---------------------------------------------------------------------------

typedef struct _TreeNode TreeNode;
struct _TreeNode {
    hz_splay_node splayNode;
    uintptr_t value;
    int info;
};

int treeNodeCompare(hz_splay_node* n1, hz_splay_node* n2) {
    TreeNode* node1 = (TreeNode*)n1;
    TreeNode* node2 = (TreeNode*)n2;
    if(node1->value < node2->value) return 1;
    else if(node1->value > node2->value) return -1;
    else return 0;
}

static int sTreeVisitCnt = 0;
void treeNodeVisit(hz_splay_tree* p, hz_splay_node* n, int dept) {
    TreeNode* node = (TreeNode*)n;
    printf("[%d] %lu, %d - %d -\n", sTreeVisitCnt++, node->value, node->info, dept);
}

void test_splay_tree() {
    hz_splay_tree* p = hz_splay_tree_create(1024*256, 1024*64, sizeof(TreeNode), treeNodeCompare);
    
    int i;
    for(i = 0; i < 100000; i++) {
        TreeNode node;
        node.value = rand()%100000;
        int isInserted = 0;
        int root = hz_splay_tree_insert(p, (hz_splay_node*)&node, &isInserted);
        if(root >= 0 && isInserted) {
            TreeNode* newNode = (TreeNode*)hz_splay_tree_get(p, root);
            if(newNode) {
                newNode->value = node.value;
                newNode->info = (int)newNode->value + 1000000;
            }
        }
    }
    printf("After insert(root=%d):\n", p->root_index);
    sTreeVisitCnt = 0;
    hz_splay_tree_traverse(p, treeNodeVisit);
    
    for(i = 0; i < 100000; i++) {
        TreeNode node;
        node.value = rand()%100000;
        hz_splay_tree_delete(p, (hz_splay_node*)&node);
    }
    printf("After delete(root=%d):\n", p->root_index);
    sTreeVisitCnt = 0;
    hz_splay_tree_traverse(p, treeNodeVisit);
    
    hz_splay_tree_destroy(p);
}

// ---------------------------------------------------------------------------

void test_stack_table() {
    hz_stack_table* p = hz_stack_table_create();
    
    void* buf[64] = {0};
    int stackNum = backtrace((void**)buf, 64);
    
    int i;
    
    printf("stack(count=%d):\n", stackNum);
    for(i = 0; i < stackNum; i++) {
        printf("[%d]0x%lx\n", i, (uintptr_t)buf[i]);
    }
    
    int index = hz_stack_table_insert(p, buf, stackNum);
    
    void* buf2[64] = {0};
    int cnt = 64;
    hz_stack_table_get(p, index, buf2, &cnt);
    
    printf("stack2(count=%d):\n", cnt);
    for(i = 0; i < cnt; i++) {
        printf("[%d]0x%lx\n", i, (uintptr_t)buf2[i]);
    }
    
    hz_stack_table_destroy(p);
    p = 0;
}

// ---------------------------------------------------------------------------

typedef struct _HeapNode HeapNode;
struct _HeapNode {
    int value;
    int info;
};

int heapNodeCompare(void* n1, void* n2) {
    HeapNode* node1 = (HeapNode*)n1;
    HeapNode* node2 = (HeapNode*)n2;
    if(node1->value > node2->value) {
        return 1;
    } else if(node1->value < node2->value) {
        return -1;
    }
    return 0;
}

void heapNodeAssign(void* dest, void* src) {
    memcpy(dest, src, sizeof(HeapNode));
}

void test_top_heap() {
    hz_top_heap* p = hz_top_heap_create(10, sizeof(HeapNode), heapNodeCompare, heapNodeAssign);
    int testData[] = {23, 33, 11, 434, 67, 45, 545, 7, 4, 49, 66, 212, 89};
    int len = sizeof(testData) / sizeof(int);
    for(int i = 0; i < len; i++) {
        HeapNode n;
        n.value = testData[i];
        n.info = testData[i] + 1000;
        hz_top_heap_insert(p, &n);
    }
    int num = 0;
    HeapNode* res = hz_top_heap_build_sort_result(p, &num);
    printf("Top heap result:\n");
    for(int i = 0; i < num; i++) {
        HeapNode* rn = res + i;
        printf("[%d] %d, %d\n", i, rn->value, rn->info);
    }
    printf("\n");
    hz_top_heap_destroy(p);
}

// ---------------------------------------------------------------------------
