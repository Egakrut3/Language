#ifndef SIMPLIFIER_H
#define SIMPLIFIER_H

#include "Common.h"
#include "Bin_tree_node.h"

Bin_tree_node *DSL_new_Bin_tree_node(Bin_tree_node *left, Bin_tree_node *right,
                                     Expression_tree_data data,
                                     errno_t *err_ptr);

Bin_tree_node *copy_subtree(Bin_tree_node *src, errno_t *err_ptr);

errno_t simplify_subtree(Bin_tree_node **dest, Bin_tree_node *src);

#endif /* SIMPLIFIER_H */
