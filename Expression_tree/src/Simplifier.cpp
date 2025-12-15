#include "Bin_tree_node.h"
#include "DSL.h"
#include <math.h>

#define FINAL_CODE

errno_t simplify_subtree(Bin_tree_node **const dest, Bin_tree_node const *const src) {
    assert(src);

    errno_t cur_err = 0;
    errno_t *const err_ptr = &cur_err;

    if (src->data.type != EXPRESSION_TREE_OPERATION_TYPE) {
        assert(!src->left); assert(!src->right);

        *dest = COPY(src);

        return 0;
    }

    Bin_tree_node *left_res  = nullptr,
                  *right_res = nullptr;
    switch (src->data.val.operation) {
        #define HANDLE_OPERATION(name, c_decl, ...)                     \
        case name ## _OPERATION:                                        \
            CHECK_FUNC(simplify_subtree, &right_res, src->right);       \
            if (right_res->data.type == EXPRESSION_TREE_LITERAL_TYPE) { \
                *dest = LITER_(c_decl(right_res->data.val.val));        \
                CHECK_FUNC(Bin_tree_node_Dtor, right_res);              \
                right_res = nullptr;                                    \
            }                                                           \
            else { *dest = name ## _(right_res); }                      \
            break;
        //This include generates cases for
        //simplifying all existing unary functions
        //by applying previously declared macros
        //HANDLE_OPERATION to them
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wfloat-equal"
        #include "Simplifier_info/Unary_arithmetic_functions.h"
        #pragma GCC diagnostic pop
        #undef HANDLE_OPERATION

        #define HANDLE_OPERATION(name, c_decl, left_neutral, right_neutral,                     \
                                 left_const_crit,  left_const_res,                              \
                                 right_const_crit, right_const_res, ...)                        \
        case name ## _OPERATION:                                                                \
            CHECK_FUNC(simplify_subtree, &left_res,  src->left);                                \
            CHECK_FUNC(simplify_subtree, &right_res, src->right);                               \
            if (left_res->data.type == EXPRESSION_TREE_LITERAL_TYPE) {                          \
                if (left_res->data.val.val == left_const_crit) {                                \
                    *dest = LITER_(left_const_res);                                             \
                    CHECK_FUNC(Bin_tree_node_Dtor, left_res);                                   \
                    CHECK_FUNC(Bin_tree_node_Dtor, right_res);                                  \
                    left_res  = nullptr;                                                        \
                    right_res = nullptr;                                                        \
                }                                                                               \
                else if (left_res->data.val.val == left_neutral) {                              \
                    *dest = right_res;                                                          \
                    CHECK_FUNC(Bin_tree_node_Dtor, left_res);                                   \
                    left_res = nullptr;                                                         \
                }                                                                               \
                else if (right_res->data.type == EXPRESSION_TREE_LITERAL_TYPE) {                \
                    *dest = LITER_(c_decl(left_res->data.val.val, right_res->data.val.val));    \
                    CHECK_FUNC(Bin_tree_node_Dtor, left_res);                                   \
                    CHECK_FUNC(Bin_tree_node_Dtor, right_res);                                  \
                    left_res  = nullptr;                                                        \
                    right_res = nullptr;                                                        \
                }                                                                               \
                else { *dest = name ## _(left_res, right_res); }                                \
            }                                                                                   \
            else if (right_res->data.type == EXPRESSION_TREE_LITERAL_TYPE) {                    \
                if (right_res->data.val.val == right_const_crit) {                              \
                    *dest = LITER_(right_const_res);                                            \
                    CHECK_FUNC(Bin_tree_node_Dtor, left_res);                                   \
                    CHECK_FUNC(Bin_tree_node_Dtor, right_res);                                  \
                    left_res  = nullptr;                                                        \
                    right_res = nullptr;                                                        \
                }                                                                               \
                else if (right_res->data.val.val == right_neutral) {                            \
                    *dest = left_res;                                                           \
                    CHECK_FUNC(Bin_tree_node_Dtor, right_res);                                  \
                    right_res = nullptr;                                                        \
                }                                                                               \
                else { *dest = name ## _(left_res, right_res); }                                \
            }                                                                                   \
            else { *dest = name ## _(left_res, right_res); }                                    \
            break;
        //This include generates cases for
        //simplifying all existing binary functions
        //by applying previously declared macros
        //HANDLE_OPERATION to them
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wfloat-equal"
        #include "Simplifier_info/Binary_arithmetic_functions.h"
        #pragma GCC diagnostic pop
        #undef HANDLE_OPERATION


        #define HANDLE_OPERATION(name, c_decl, left_neutral, right_neutral,                 \
                                 left_const_crit,  left_const_res,                          \
                                 right_const_crit, right_const_res, ...)                    \
        case name ## _OPERATION:                                                            \
            CHECK_FUNC(simplify_subtree, &left_res,  src->left);                            \
            CHECK_FUNC(simplify_subtree, &right_res, src->right);                           \
            if (left_res->data.type == EXPRESSION_TREE_LITERAL_TYPE) {                      \
                if (left_res->data.val.val == left_const_crit) {                            \
                    *dest = LITER_(left_const_res);                                         \
                    CHECK_FUNC(Bin_tree_node_Dtor, left_res);                               \
                    CHECK_FUNC(Bin_tree_node_Dtor, right_res);                              \
                    left_res  = nullptr;                                                    \
                    right_res = nullptr;                                                    \
                }                                                                           \
                else if (left_res->data.val.val == left_neutral) {                          \
                    *dest = right_res;                                                      \
                    CHECK_FUNC(Bin_tree_node_Dtor, left_res);                               \
                    left_res = nullptr;                                                     \
                }                                                                           \
                else if (right_res->data.type == EXPRESSION_TREE_LITERAL_TYPE) {            \
                    *dest = LITER_(left_res->data.val.val c_decl right_res->data.val.val);  \
                    CHECK_FUNC(Bin_tree_node_Dtor, left_res);                               \
                    CHECK_FUNC(Bin_tree_node_Dtor, right_res);                              \
                    left_res  = nullptr;                                                    \
                    right_res = nullptr;                                                    \
                }                                                                           \
                else { *dest = name ## _(left_res, right_res); }                            \
            }                                                                               \
            else if (right_res->data.type == EXPRESSION_TREE_LITERAL_TYPE) {                \
                if (right_res->data.val.val == right_const_crit) {                          \
                    *dest = LITER_(right_const_res);                                        \
                    CHECK_FUNC(Bin_tree_node_Dtor, left_res);                               \
                    CHECK_FUNC(Bin_tree_node_Dtor, right_res);                              \
                    left_res  = nullptr;                                                    \
                    right_res = nullptr;                                                    \
                }                                                                           \
                else if (right_res->data.val.val == right_neutral) {                        \
                    *dest = left_res;                                                       \
                    CHECK_FUNC(Bin_tree_node_Dtor, right_res);                              \
                    right_res = nullptr;                                                    \
                }                                                                           \
                else { *dest = name ## _(left_res, right_res); }                            \
            }                                                                               \
            else { *dest = name ## _(left_res, right_res); }                                \
            break;
        //This include generates cases for
        //simplifying all existing binary operators
        //by applying previously declared macros
        //HANDLE_OPERATION to them
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wfloat-equal"
        #include "Simplifier_info/Binary_arithmetic_operators.h"
        #pragma GCC diagnostic pop
        #undef HANDLE_OPERATION

        default:
            PRINT_LINE();
            abort();
    }

    return 0;
}

#undef FINAL_CODE
