#include "Bin_tree_node.h"

#define FINAL_CODE

errno_t make_assembler(FILE *const out_stream, Bin_tree_node const *const src) {
    assert(out_stream); assert(src);

    errno_t verify_err = 0;
    CHECK_FUNC(subtree_verify, &verify_err, src);
    if (verify_err) { return verify_err; }

    if (
}

#undef FINAL_CODE
