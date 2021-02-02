#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#define NDEBUG // (Optional, see assert(3).)
#include <assert.h>
#define RB_COMPACT // (Optional, embed color bits in right-child pointers.)
#include "rb.h"



struct test_sruct {
    int value;
    rb_node(struct test_sruct) link_node;
};

typedef rb_tree(struct test_sruct) rt_rbtree_t;


rt_rbtree_t tree_root;

rb_proto(static, rt_rbtree_, rt_rbtree_t, struct test_sruct);

static int _my_rbtree_cmp(const struct test_sruct *a_node, const struct test_sruct *a_other)
{
    if(a_node->value <  a_other->value) return -1;
    else if(a_node->value == a_other->value) return 0;
    else if(a_node->value >  a_other->value) return 1;
}


rb_gen(static, rt_rbtree_, rt_rbtree_t, struct test_sruct, link_node, _my_rbtree_cmp);


struct test_sruct *_iter_cb(rt_rbtree_t *tree, struct test_sruct *node, void *arg) {
    printf("_iter_cb value = %d\n", node->value);
    return rt_rbtree_next(&tree_root, node);
}


int main()
{
    int test_number = 64;
    int i;
    struct test_sruct test_data[test_number];
    struct test_sruct *test;
    
    for(i=0; i<test_number; i++) {
        test_data[i].value = i+1;
    }

    rt_rbtree_new(&tree_root);

    if(rt_rbtree_empty(&tree_root)) {
        printf("is empty.\n");
    }
    for(i=0; i<test_number; i++) {
        rt_rbtree_insert(&tree_root, &test_data[i]);
    }
    for(i=0; i<test_number; i+=3) {
        rt_rbtree_remove(&tree_root, &test_data[i]);
    }

    for(test = rt_rbtree_first(&tree_root); test; test = rt_rbtree_next(&tree_root, test)) {
        printf("value = %d\n", test->value);
    }
    
    rt_rbtree_iter(&tree_root, rt_rbtree_first(&tree_root), _iter_cb, NULL);

    rt_rbtree_destroy(&tree_root, NULL, NULL);
    return 0;
}
