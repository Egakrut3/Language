#ifndef DSL_H
#define DSL_H

#define COPY(src) copy_subtree(src, err_ptr)

#define LITER_(lit_val)                                                                 \
DSL_new_Bin_tree_node(nullptr, nullptr,                                                 \
                      Expression_tree_data{EXPRESSION_TREE_LITERAL_TYPE,                \
                                           Expression_tree_node_val{.val = lit_val}},   \
                      err_ptr)

#define ADD_(left, right)                                                                           \
DSL_new_Bin_tree_node(left, right,                                                                  \
                      Expression_tree_data{EXPRESSION_TREE_OPERATION_TYPE,                          \
                                            Expression_tree_node_val{.operation = ADD_OPERATION}},  \
                      err_ptr)
#define SUB_(left, right)                                                                           \
DSL_new_Bin_tree_node(left, right,                                                                  \
                      Expression_tree_data{EXPRESSION_TREE_OPERATION_TYPE,                          \
                                            Expression_tree_node_val{.operation = SUB_OPERATION}},  \
                      err_ptr)

#define MLT_(left, right)                                                                           \
DSL_new_Bin_tree_node(left, right,                                                                  \
                      Expression_tree_data{EXPRESSION_TREE_OPERATION_TYPE,                          \
                                            Expression_tree_node_val{.operation = MLT_OPERATION}},  \
                      err_ptr)
#define DIV_(left, right)                                                                           \
DSL_new_Bin_tree_node(left, right,                                                                  \
                      Expression_tree_data{EXPRESSION_TREE_OPERATION_TYPE,                          \
                                            Expression_tree_node_val{.operation = DIV_OPERATION}},  \
                      err_ptr)

#define LN_(src)                                                                                    \
DSL_new_Bin_tree_node(nullptr, src,                                                                 \
                      Expression_tree_data{EXPRESSION_TREE_OPERATION_TYPE,                          \
                                           Expression_tree_node_val{.operation = LN_OPERATION}},    \
                      err_ptr)
#define POW_(left, right)                                                                           \
DSL_new_Bin_tree_node(left, right,                                                                  \
                      Expression_tree_data{EXPRESSION_TREE_OPERATION_TYPE,                          \
                                            Expression_tree_node_val{.operation = POW_OPERATION}},  \
                      err_ptr)

#define SIN_(src)                                                                                   \
DSL_new_Bin_tree_node(nullptr, src,                                                                 \
                      Expression_tree_data{EXPRESSION_TREE_OPERATION_TYPE,                          \
                                           Expression_tree_node_val{.operation = SIN_OPERATION}},   \
                      err_ptr)
#define COS_(src)                                                                                   \
DSL_new_Bin_tree_node(nullptr, src,                                                                 \
                      Expression_tree_data{EXPRESSION_TREE_OPERATION_TYPE,                          \
                                           Expression_tree_node_val{.operation = COS_OPERATION}},   \
                      err_ptr)

#endif
