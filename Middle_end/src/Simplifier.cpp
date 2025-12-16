#include "Simplifier.h"
#include "DSL.h"
#include <string.h>
#include <math.h>

#define FINAL_CODE

Bin_tree_node *DSL_new_Bin_tree_node(Bin_tree_node *const left, Bin_tree_node *const right,
                                     Expression_tree_data const data,
                                     errno_t *const err_ptr) {
    assert(err_ptr);

    Bin_tree_node *result = nullptr;
    *err_ptr = My_calloc((void **)&result, 1, sizeof(Bin_tree_node));
    *err_ptr = Bin_tree_node_Ctor(result, left, right, data);
    return result;
}

Bin_tree_node *copy_subtree(Bin_tree_node *const src, errno_t *const err_ptr) {
    if (!src) { return nullptr; }

    int verify_err = subtree_verify(err_ptr, src);
    if (verify_err) { return nullptr; }
    if (*err_ptr)   { return nullptr; }

    if (src->data.type == EXPRESSION_TREE_ID_TYPE) {
        return DSL_new_Bin_tree_node(COPY(src->left), COPY(src->right),
                                     Expression_tree_data{
                                     EXPRESSION_TREE_ID_TYPE,
                                     Expression_tree_node_val{.name = strdup(src->data.val.name)}},
                                     err_ptr);
    }
    else { return DSL_new_Bin_tree_node(COPY(src->left), COPY(src->right), src->data, err_ptr); }
}

errno_t simplify_subtree(Bin_tree_node **const dest, Bin_tree_node *const src) {
    assert(src);

    errno_t verify_err = 0;
    CHECK_FUNC(subtree_verify, &verify_err, src);
    if (verify_err) { return verify_err; }

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
                CHECK_FUNC(delete_Bin_tree_node, right_res);            \
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
        #include "Simplifier_info/Unary_functions.h"
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
                    CHECK_FUNC(delete_Bin_tree_node, left_res);                                 \
                    CHECK_FUNC(delete_Bin_tree_node, right_res);                                \
                    left_res  = nullptr;                                                        \
                    right_res = nullptr;                                                        \
                }                                                                               \
                else if (left_res->data.val.val == left_neutral) {                              \
                    *dest = right_res;                                                          \
                    CHECK_FUNC(delete_Bin_tree_node, left_res);                                 \
                    left_res = nullptr;                                                         \
                }                                                                               \
                else if (right_res->data.type == EXPRESSION_TREE_LITERAL_TYPE) {                \
                    *dest = LITER_(c_decl(left_res->data.val.val, right_res->data.val.val));    \
                    CHECK_FUNC(delete_Bin_tree_node, left_res);                                 \
                    CHECK_FUNC(delete_Bin_tree_node, right_res);                                \
                    left_res  = nullptr;                                                        \
                    right_res = nullptr;                                                        \
                }                                                                               \
                else { *dest = name ## _(left_res, right_res); }                                \
            }                                                                                   \
            else if (right_res->data.type == EXPRESSION_TREE_LITERAL_TYPE) {                    \
                if (right_res->data.val.val == right_const_crit) {                              \
                    *dest = LITER_(right_const_res);                                            \
                    CHECK_FUNC(delete_Bin_tree_node, left_res);                                 \
                    CHECK_FUNC(delete_Bin_tree_node, right_res);                                \
                    left_res  = nullptr;                                                        \
                    right_res = nullptr;                                                        \
                }                                                                               \
                else if (right_res->data.val.val == right_neutral) {                            \
                    *dest = left_res;                                                           \
                    CHECK_FUNC(delete_Bin_tree_node, right_res);                                \
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
        #include "Simplifier_info/Binary_functions.h"
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
                    CHECK_FUNC(delete_Bin_tree_node, left_res);                             \
                    CHECK_FUNC(delete_Bin_tree_node, right_res);                            \
                    left_res  = nullptr;                                                    \
                    right_res = nullptr;                                                    \
                }                                                                           \
                else if (left_res->data.val.val == left_neutral) {                          \
                    *dest = right_res;                                                      \
                    CHECK_FUNC(delete_Bin_tree_node, left_res);                             \
                    left_res = nullptr;                                                     \
                }                                                                           \
                else if (right_res->data.type == EXPRESSION_TREE_LITERAL_TYPE) {            \
                    *dest = LITER_(left_res->data.val.val c_decl right_res->data.val.val);  \
                    CHECK_FUNC(delete_Bin_tree_node, left_res);                             \
                    CHECK_FUNC(delete_Bin_tree_node, right_res);                            \
                    left_res  = nullptr;                                                    \
                    right_res = nullptr;                                                    \
                }                                                                           \
                else { *dest = name ## _(left_res, right_res); }                            \
            }                                                                               \
            else if (right_res->data.type == EXPRESSION_TREE_LITERAL_TYPE) {                \
                if (right_res->data.val.val == right_const_crit) {                          \
                    *dest = LITER_(right_const_res);                                        \
                    CHECK_FUNC(delete_Bin_tree_node, left_res);                             \
                    CHECK_FUNC(delete_Bin_tree_node, right_res);                            \
                    left_res  = nullptr;                                                    \
                    right_res = nullptr;                                                    \
                }                                                                           \
                else if (right_res->data.val.val == right_neutral) {                        \
                    *dest = left_res;                                                       \
                    CHECK_FUNC(delete_Bin_tree_node, right_res);                            \
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
        #include "Simplifier_info/Binary_operators.h"
        #pragma GCC diagnostic pop
        #undef HANDLE_OPERATION

        #define HANDLE_OPERATION(name, ...) \
        case name ## _OPERATION:            \
            break;
        //This include generates cases for
        //all existing not ariphmetic operators
        //by applying previously declared macros
        //HANDLE_OPERATION to them
        #include "Simplifier_info/Others.h"
        #undef HANDLE_OPERATION

        default:
            PRINT_LINE();
            abort();
    }

    return 0;
}

#undef FINAL_CODE
