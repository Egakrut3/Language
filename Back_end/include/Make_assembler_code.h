#ifndef MAKE_ASSEMBLER_CODE_H
#define MAKE_ASSEMBLER_CODE_H

#include "Bin_tree_node.h"

#define INVALID_AST 10'000
errno_t make_assembler_code(FILE *out_stream, Bin_tree_node *src);

#endif /* MAKE_ASSEMBLER_CODE_H */
