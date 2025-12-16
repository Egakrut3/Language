#ifndef BIN_TREE_NODE_H
#define BIN_TREE_NODE_H

#include "Common.h"

enum Expression_tree_node_type {
    EXPRESSION_TREE_LITERAL_TYPE,
    EXPRESSION_TREE_OPERATION_TYPE,
    EXPRESSION_TREE_ID_TYPE,
};

enum Expression_tree_operation {
    #define HANDLE_OPERATION(name, ...) \
    name ## _OPERATION,
    //This includes generates enum-states for all
    //operations by applying previously declared
    //macros HANDLE_OPERATION to them
    #include "Operation_list.h"
    #undef HANDLE_OPERATION
};

union Expression_tree_node_val {
    double                    val;
    Expression_tree_operation operation;
    char                      *name;
};

struct Expression_tree_data {
    Expression_tree_node_type type;
    Expression_tree_node_val  val;
};

struct Bin_tree_node {
    Bin_tree_node        *left,
                         *right;

    Expression_tree_data data;

    bool                 is_valid;
    bool                 verify_used; //I don't want to use hash-map in verify, because it is hard
};

errno_t Bin_tree_node_Ctor(Bin_tree_node *node_ptr,
                           Bin_tree_node *left, Bin_tree_node *right,
                           Expression_tree_data data);
errno_t new_Bin_tree_node(Bin_tree_node **dest,
                          Bin_tree_node *left, Bin_tree_node *right,
                          Expression_tree_data data);
Bin_tree_node *DSL_new_Bin_tree_node(Bin_tree_node *left, Bin_tree_node *right,
                                     Expression_tree_data data,
                                     errno_t *err_ptr);

errno_t Bin_tree_node_Dtor(Bin_tree_node *node_ptr);
errno_t delete_Bin_tree_node(Bin_tree_node **dest);

#define TREE_NODE_INVALID     0B10'000'000'000
#define TREE_NODE_VERIFY_USED 0B100'000'000'000
errno_t Bin_tree_node_verify(errno_t *err_ptr, Bin_tree_node const *node_ptr);

errno_t subtree_Dtor(Bin_tree_node *node_ptr);

#define TREE_INVALID_STRUCTURE 0B1'000'000'000'000
errno_t subtree_verify(errno_t *err_ptr, Bin_tree_node *node_ptr);

errno_t subtree_dot_dump(FILE *out_stream, Bin_tree_node const *node_ptr);
errno_t subtree_text_dump(FILE *out_stream, Bin_tree_node const *src);

#define INCORRECT_TREE_INPUT 1'000
errno_t str_prefix_read_subtree(Bin_tree_node **dest, char const *buffer);

Bin_tree_node *copy_subtree(Bin_tree_node const *src, errno_t *err_ptr);
errno_t simplify_subtree(Bin_tree_node **dest, Bin_tree_node const *src);

#endif /* BIN_TREE_NODE_H */
