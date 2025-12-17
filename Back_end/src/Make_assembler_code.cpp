#include "Make_assembler_code.h"
#include "Name_table.h"

#define FINAL_CODE

struct Translator {
    Name_table var_table,
               func_table,
               proc_table;
};

static errno_t get_function_arguments(size_t *const dest, Bin_tree_node *const cur_node) {
    assert(cur_node);
}

static errno_t set_functions_table(Translator *const trans_ptr, Bin_tree_node *const cur_node) {
    assert(cur_node);

    if (cur_node->data.type != EXPRESSION_TREE_OPERATION_TYPE) { return INVALID_AST; }

    if (cur_node->data.val.operation == FUNC_OPERATION) {
        assert(
    }
}

errno_t make_assembler_code(FILE *const out_stream, Bin_tree_node const *const src) {
    assert(out_stream); assert(src);

    errno_t verify_err = 0;
    CHECK_FUNC(subtree_verify, &verify_err, src);
    if (verify_err) { return verify_err; }

    if (
}

#undef FINAL_CODE
