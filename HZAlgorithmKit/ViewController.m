//
//  ViewController.m
//  HZAlgorithmKit
//
//  Created by hot on 19/1/19.
//  Copyright © 2019年 hot. All rights reserved.
//

#import "ViewController.h"
#import "hz_splay_tree.h"
#import "hz_stack_table.h"
#import "hz_top_heap.h"

void test_splay_tree() {
    
}

// ---------------------------------------------------------------------------

void test_stack_table() {
    
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

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    //test_splay_tree();
    //test_stack_table();
    test_top_heap();
}


@end
