#include "Make_assembler_code.h"
#include "Name_table.h"

#define FINAL_CODE

#define FUNC_SCOPE 0B1
#define PROC_SCOPE 0B10

struct Translator {
    Name_table func_table,
               proc_table,
               var_table;
    size_t     labels_count,
               var_count,
               cur_scope;

    bool       is_valid;
};

static errno_t Translator_Ctor(Translator *const dest) {
    assert(dest); assert(!dest->is_valid);

    CHECK_FUNC(Name_table_Ctor, &dest->func_table);
    CHECK_FUNC(Name_table_Ctor, &dest->proc_table);
    dest->labels_count = 0;
    dest->var_count    = 0;
    dest->cur_scope    = 0;

    dest->is_valid = true;

    return 0;
}

static errno_t Translator_Dtor(Translator *const trans_ptr) {
    assert(trans_ptr); assert(trans_ptr->is_valid);

    CHECK_FUNC(Name_table_Dtor, &trans_ptr->func_table);
    CHECK_FUNC(Name_table_Dtor, &trans_ptr->proc_table);

    trans_ptr->is_valid = false;

    return 0;
}

static errno_t get_function_info(size_t *const dest_count, Name_table *const dest_table,
                                 Bin_tree_node *const cur_node) {
    assert(cur_node); assert(dest_count);

    switch (cur_node->data.type) {
        case EXPRESSION_TREE_LITERAL_TYPE:
            return INVALID_AST;

        case EXPRESSION_TREE_OPERATION_TYPE:
            if (cur_node->data.val.operation != CONA_OPERATION) { return INVALID_AST; }

            CHECK_FUNC(get_function_info, dest_count, dest_table, cur_node->left);
            CHECK_FUNC(get_function_info, dest_count, dest_table, cur_node->right);

            return 0;

        case EXPRESSION_TREE_ID_TYPE:
            if (cur_node->left)  { return INVALID_AST; }
            if (cur_node->right) { return INVALID_AST; }

            if (dest_table) { CHECK_FUNC(Name_table_set, dest_table, cur_node->data.val.name, *dest_count); }
            *dest_count += 1;
            return 0;

        default:
            PRINT_LINE();
            abort();
    }

    PRINT_LINE();
    abort();
}

static errno_t set_functions_table(Translator *const trans_ptr, Bin_tree_node *const cur_node) {
    assert(trans_ptr); assert(cur_node);

    if (cur_node->data.type != EXPRESSION_TREE_OPERATION_TYPE) { return INVALID_AST; }

    if (cur_node->data.val.operation == FUNC_OPERATION) {
        if (!cur_node->left or
            cur_node->left->data.type != EXPRESSION_TREE_OPERATION_TYPE or
            cur_node->left->data.val.operation != FINF_OPERATION) { return INVALID_AST; }
        if (!cur_node->left->left or
            cur_node->left->left->data.type != EXPRESSION_TREE_ID_TYPE) { return INVALID_AST; }
        if (cur_node->left->left->left)  { return INVALID_AST; }
        if (cur_node->left->left->right) { return INVALID_AST; }
        if (!cur_node->left->right)      { return INVALID_AST; }

        size_t arg_count = 0;
        CHECK_FUNC(get_function_info, &arg_count, nullptr, cur_node->left->right);

        CHECK_FUNC(Name_table_set, &trans_ptr->func_table, cur_node->left->left->data.val.name, arg_count);
        CHECK_FUNC(Name_table_set, &trans_ptr->proc_table, cur_node->left->left->data.val.name, -1);

        return 0;
    }

    if (cur_node->data.val.operation == PROC_OPERATION) {
        if (!cur_node->left or
            cur_node->left->data.type != EXPRESSION_TREE_OPERATION_TYPE or
            cur_node->left->data.val.operation != FINF_OPERATION) { return INVALID_AST; }
        if (!cur_node->left->left or
            cur_node->left->left->data.type != EXPRESSION_TREE_ID_TYPE) { return INVALID_AST; }
        if (cur_node->left->left->left)  { return INVALID_AST; }
        if (cur_node->left->left->right) { return INVALID_AST; }
        if (!cur_node->left->right)      { return INVALID_AST; }

        size_t arg_count = 0;
        CHECK_FUNC(get_function_info, &arg_count, nullptr, cur_node->left->right);

        CHECK_FUNC(Name_table_set, &trans_ptr->func_table, cur_node->left->left->data.val.name, -1);
        CHECK_FUNC(Name_table_set, &trans_ptr->proc_table, cur_node->left->left->data.val.name, arg_count);

        return 0;
    }

    if (cur_node->data.val.operation == CONL_OPERATION) {
        if (!cur_node->left->left)  { return INVALID_AST; }
        if (!cur_node->left->right) { return INVALID_AST; }

        CHECK_FUNC(set_functions_table, trans_ptr, cur_node->left);
        CHECK_FUNC(set_functions_table, trans_ptr, cur_node->right);

        return 0;
    }

    return INVALID_AST;
}

static errno_t following_make_assembler_code(FILE *out_stream, bool *need_pop,
                                             Translator *trans_ptr, Bin_tree_node *cur_node);

static errno_t move_variables(FILE *const out_stream, size_t *const remain_args, size_t const total_args,
                              Translator *const trans_ptr, Bin_tree_node *const cur_node) {
    assert(out_stream); assert(remain_args); assert(*remain_args <= total_args);
    assert(trans_ptr); assert(cur_node);

    if (!remain_args) { return INVALID_AST; }

    if (cur_node->data.type == EXPRESSION_TREE_OPERATION_TYPE and
        cur_node->data.val.operation == CONA_OPERATION) {
        if (!cur_node->left->left)  { return INVALID_AST; }
        if (!cur_node->left->right) { return INVALID_AST; }

        CHECK_FUNC(move_variables, out_stream, remain_args, total_args, trans_ptr, cur_node->left);
        CHECK_FUNC(move_variables, out_stream, remain_args, total_args, trans_ptr, cur_node->right);

        return 0;
    }

    bool cur_need_pop = false;

    CHECK_FUNC(following_make_assembler_code, out_stream, &cur_need_pop, trans_ptr, cur_node);
    if (!cur_need_pop) { return INVALID_AST; }
    fprintf_s(out_stream, "PUSHR r1\n");
    fprintf_s(out_stream, "PUSH %zu\n", total_args - *remain_args);
    fprintf_s(out_stream, "ADD\n");
    fprintf_s(out_stream, "POPR r0\n");
    fprintf_s(out_stream, "POPM r0\n");
    *remain_args -= 1;

    return 0;
}

static errno_t following_make_assembler_code(FILE *const out_stream, bool *const need_pop,
                                             Translator *const trans_ptr, Bin_tree_node *const cur_node) {
    assert(out_stream); assert(need_pop); assert(trans_ptr); assert(cur_node);

    bool cur_need_pop = false;

    if (cur_node->data.type == EXPRESSION_TREE_OPERATION_TYPE and
        cur_node->data.val.operation == CONL_OPERATION) {
        if (!cur_node->left->left)  { return INVALID_AST; }
        if (!cur_node->left->right) { return INVALID_AST; }

        CHECK_FUNC(following_make_assembler_code, out_stream, &cur_need_pop, trans_ptr, cur_node->left);
        if (cur_need_pop) { fprintf_s(out_stream, "POP\n"); }
        CHECK_FUNC(following_make_assembler_code, out_stream, &cur_need_pop, trans_ptr, cur_node->right);
        if (cur_need_pop) { fprintf_s(out_stream, "POP\n"); }

        *need_pop = false;
        return 0;
    }

    if (!trans_ptr->cur_scope) {
        switch (cur_node->data.type) {
            case EXPRESSION_TREE_LITERAL_TYPE:
                return INVALID_AST;

            case EXPRESSION_TREE_OPERATION_TYPE:
                if (cur_node->data.val.operation == FUNC_OPERATION) {
                    fprintf_s(out_stream, ":%s\n", cur_node->left->left->data.val.name);

                    CHECK_FUNC(Name_table_Ctor, &trans_ptr->var_table);
                    CHECK_FUNC(get_function_info, &trans_ptr->var_count, &trans_ptr->var_table,
                                                  cur_node->left->right);
                    trans_ptr->cur_scope |= FUNC_SCOPE;
                    CHECK_FUNC(following_make_assembler_code, out_stream, &cur_need_pop,
                                                              trans_ptr, cur_node->right);
                    if (cur_need_pop) { return INVALID_AST; }
                    trans_ptr->cur_scope &= ~FUNC_SCOPE;
                    trans_ptr->var_count = 0;
                    CHECK_FUNC(Name_table_Dtor, &trans_ptr->var_table);

                    *need_pop = false;
                    return 0;
                }

                if (cur_node->data.val.operation == PROC_OPERATION) {
                    fprintf_s(out_stream, ":%s\n", cur_node->left->left->data.val.name);

                    CHECK_FUNC(Name_table_Ctor, &trans_ptr->var_table);
                    CHECK_FUNC(get_function_info, &trans_ptr->var_count, &trans_ptr->var_table,
                                                  cur_node->left->right);
                    trans_ptr->cur_scope |= PROC_SCOPE;
                    CHECK_FUNC(following_make_assembler_code, out_stream, &cur_need_pop,
                                                              trans_ptr, cur_node->right);
                    if (cur_need_pop) { return INVALID_AST; }
                    trans_ptr->cur_scope &= ~PROC_SCOPE;
                    trans_ptr->var_count = 0;
                    CHECK_FUNC(Name_table_Dtor, &trans_ptr->var_table);

                    *need_pop = false;
                    return 0;
                }

                return INVALID_AST;

            case EXPRESSION_TREE_ID_TYPE:
                return INVALID_AST;

            default:
                PRINT_LINE();
                abort();
        }
    }

    if (trans_ptr->cur_scope & FUNC_SCOPE) {
        if (cur_node->data.type == EXPRESSION_TREE_OPERATION_TYPE and
            cur_node->data.val.operation == RET_OPERATION) {
            if (cur_node->left) { return INVALID_AST; }

            CHECK_FUNC(following_make_assembler_code, out_stream, &cur_need_pop,
                                                      trans_ptr, cur_node->right);
            if (!cur_need_pop) { return INVALID_AST; }
            fprintf_s(out_stream, "POPR r0\n");
            fprintf_s(out_stream, "RET\n");

            *need_pop = false;
            return 0;
        }
    }

    if (trans_ptr->cur_scope & PROC_SCOPE) {
        if (cur_node->data.type == EXPRESSION_TREE_OPERATION_TYPE and
            cur_node->data.val.operation == FIN_OPERATION) {
            if (cur_node->left)  { return INVALID_AST; }
            if (cur_node->right) { return INVALID_AST; }

            fprintf_s(out_stream, "RET\n");

            *need_pop = false;
            return 0;
        }
    }

    size_t cur_ind = 0;
    switch (cur_node->data.type) {
        case EXPRESSION_TREE_LITERAL_TYPE:
            if (cur_node->left)  { return INVALID_AST; }
            if (cur_node->right) { return INVALID_AST; }

            fprintf_s(out_stream, "PUSH %lG\n", cur_node->data.val.val);

            *need_pop = true;
            return 0;

        case EXPRESSION_TREE_OPERATION_TYPE:
            if (cur_node->data.val.operation == VAR_OPERATION) {
                if (!cur_node->left or
                    cur_node->left->data.type != EXPRESSION_TREE_ID_TYPE) { return INVALID_AST; }
                if (cur_node->left->left)  { return INVALID_AST; }
                if (cur_node->left->right) { return INVALID_AST; }
                if (!cur_node->right)      { return INVALID_AST; }

                CHECK_FUNC(Name_table_set, &trans_ptr->var_table,
                                           cur_node->left->data.val.name, trans_ptr->var_count);
                CHECK_FUNC(following_make_assembler_code, out_stream, &cur_need_pop,
                                                          trans_ptr, cur_node->right);
                if (!cur_need_pop) { return INVALID_AST; }
                fprintf_s(out_stream, "PUSHR r%zu\n", FRAME_REG);
                fprintf_s(out_stream, "PUSH %zu\n", trans_ptr->var_count);
                fprintf_s(out_stream, "ADD\n");
                fprintf_s(out_stream, "POPR r0\n");
                fprintf_s(out_stream, "POPM r0\n");
                trans_ptr->var_count += 1;

                *need_pop = false;
                return 0;
            }

            if (cur_node->data.val.operation == CALL_OPERATION) {
                if (!cur_node->left or
                    cur_node->left->data.type != EXPRESSION_TREE_ID_TYPE) { return INVALID_AST; }
                if (cur_node->left->left)  { return INVALID_AST; }
                if (cur_node->left->right) { return INVALID_AST; }
                if (!cur_node->right)      { return INVALID_AST; }

                size_t cnt_args = 0;
                CHECK_FUNC(Name_table_get, &cnt_args, &trans_ptr->func_table, cur_node->left->data.val.name);
                if (cnt_args == (size_t)-1) {
                    CHECK_FUNC(Name_table_get, &cnt_args, &trans_ptr->proc_table, cur_node->left->data.val.name);
                    fprintf_s(out_stream, "PUSHR r%zu\n", FRAME_REG);
                    fprintf_s(out_stream, "PUSH %zu\n", trans_ptr->var_count);
                    fprintf_s(out_stream, "ADD\n");
                    fprintf_s(out_stream, "POPR r1\n");
                    CHECK_FUNC(move_variables, out_stream, &cnt_args, cnt_args, trans_ptr, cur_node->right);

                    fprintf_s(out_stream, "PUSHR r1\n");
                    fprintf_s(out_stream, "POPR r%zu\n", FRAME_REG);
                    fprintf_s(out_stream, "CALL ::%s\n", cur_node->left->data.val.name);
                    fprintf_s(out_stream, "PUSHR r%zu\n", FRAME_REG);
                    fprintf_s(out_stream, "PUSH %zu\n", trans_ptr->var_count);
                    fprintf_s(out_stream, "SUB\n");
                    fprintf_s(out_stream, "POPR r%zu\n", FRAME_REG);

                    *need_pop = false;
                    return 0;
                }
                fprintf_s(out_stream, "PUSHR r%zu\n", FRAME_REG);
                fprintf_s(out_stream, "PUSH %zu\n", trans_ptr->var_count);
                fprintf_s(out_stream, "ADD\n");
                fprintf_s(out_stream, "POPR r1\n");
                CHECK_FUNC(move_variables, out_stream, &cnt_args, cnt_args, trans_ptr, cur_node->right);

                fprintf_s(out_stream, "PUSHR r1\n");
                fprintf_s(out_stream, "POPR r%zu\n", FRAME_REG);
                fprintf_s(out_stream, "CALL ::%s\n", cur_node->left->data.val.name);
                fprintf_s(out_stream, "PUSHR r0\n");
                fprintf_s(out_stream, "PUSHR r%zu\n", FRAME_REG);
                fprintf_s(out_stream, "PUSH %zu\n", trans_ptr->var_count);
                fprintf_s(out_stream, "SUB\n");
                fprintf_s(out_stream, "POPR r%zu\n", FRAME_REG);

                *need_pop = true;
                return 0;
            }

            if (cur_node->data.val.operation == IF_OPERATION) {
                if (!cur_node->left)  { return INVALID_AST; }
                if (!cur_node->right) { return INVALID_AST; }

                CHECK_FUNC(following_make_assembler_code, out_stream, &cur_need_pop,
                                                          trans_ptr, cur_node->left);
                if (!cur_need_pop) { return INVALID_AST; }

                fprintf_s(out_stream, "PUSH 0\n", trans_ptr->labels_count);
                fprintf_s(out_stream, "JE ::%zu\n", trans_ptr->labels_count);
                CHECK_FUNC(following_make_assembler_code, out_stream, &cur_need_pop,
                                                          trans_ptr, cur_node->right);
                if (cur_need_pop) { fprintf_s(out_stream, "POP\n"); }
                fprintf_s(out_stream, ":%zu\n", trans_ptr->labels_count);
                trans_ptr->labels_count += 1;

                *need_pop = false;
                return 0;
            }

            if (cur_node->data.val.operation == ADD_OPERATION) {
                if (!cur_node->left)  { return INVALID_AST; }
                if (!cur_node->right) { return INVALID_AST; }

                CHECK_FUNC(following_make_assembler_code, out_stream, &cur_need_pop,
                                                          trans_ptr, cur_node->left);
                if (!cur_need_pop) { return INVALID_AST; }
                CHECK_FUNC(following_make_assembler_code, out_stream, &cur_need_pop,
                                                          trans_ptr, cur_node->right);
                if (!cur_need_pop) { return INVALID_AST; }
                fprintf_s(out_stream, "ADD\n");

                *need_pop = true;
                return 0;
            }

            if (cur_node->data.val.operation == SUB_OPERATION) {
                if (!cur_node->left)  { return INVALID_AST; }
                if (!cur_node->right) { return INVALID_AST; }

                CHECK_FUNC(following_make_assembler_code, out_stream, &cur_need_pop,
                                                          trans_ptr, cur_node->left);
                if (!cur_need_pop) { return INVALID_AST; }
                CHECK_FUNC(following_make_assembler_code, out_stream, &cur_need_pop,
                                                          trans_ptr, cur_node->right);
                if (!cur_need_pop) { return INVALID_AST; }
                fprintf_s(out_stream, "SUB\n");

                *need_pop = true;
                return 0;
            }

            if (cur_node->data.val.operation == MLT_OPERATION) {
                if (!cur_node->left)  { return INVALID_AST; }
                if (!cur_node->right) { return INVALID_AST; }

                CHECK_FUNC(following_make_assembler_code, out_stream, &cur_need_pop,
                                                          trans_ptr, cur_node->left);
                if (!cur_need_pop) { return INVALID_AST; }
                CHECK_FUNC(following_make_assembler_code, out_stream, &cur_need_pop,
                                                          trans_ptr, cur_node->right);
                if (!cur_need_pop) { return INVALID_AST; }
                fprintf_s(out_stream, "MLT\n");

                *need_pop = true;
                return 0;
            }

            if (cur_node->data.val.operation == DIV_OPERATION) {
                if (!cur_node->left)  { return INVALID_AST; }
                if (!cur_node->right) { return INVALID_AST; }

                CHECK_FUNC(following_make_assembler_code, out_stream, &cur_need_pop,
                                                          trans_ptr, cur_node->left);
                if (!cur_need_pop) { return INVALID_AST; }
                CHECK_FUNC(following_make_assembler_code, out_stream, &cur_need_pop,
                                                          trans_ptr, cur_node->right);
                if (!cur_need_pop) { return INVALID_AST; }
                fprintf_s(out_stream, "DIV\n");

                *need_pop = true;
                return 0;
            }

            if (cur_node->data.val.operation == POW_OPERATION) {
                if (!cur_node->left)  { return INVALID_AST; }
                if (!cur_node->right) { return INVALID_AST; }

                CHECK_FUNC(following_make_assembler_code, out_stream, &cur_need_pop,
                                                          trans_ptr, cur_node->left);
                if (!cur_need_pop) { return INVALID_AST; }
                CHECK_FUNC(following_make_assembler_code, out_stream, &cur_need_pop,
                                                          trans_ptr, cur_node->right);
                if (!cur_need_pop) { return INVALID_AST; }
                fprintf_s(out_stream, "POW\n");

                *need_pop = true;
                return 0;
            }

            return INVALID_AST;

        case EXPRESSION_TREE_ID_TYPE:
            if (cur_node->left)  { return INVALID_AST; }
            if (cur_node->right) { return INVALID_AST; }

            fprintf_s(out_stream, "PUSHR r%zu\n", FRAME_REG);
            CHECK_FUNC(Name_table_get, &cur_ind, &trans_ptr->var_table, cur_node->data.val.name);
            fprintf_s(out_stream, "PUSH %zu\n", cur_ind);
            fprintf_s(out_stream, "ADD\n");
            fprintf_s(out_stream, "POPR r0\n");
            fprintf_s(out_stream, "PUSHM r0\n");

            *need_pop = true;
            return 0;

        default:
            PRINT_LINE();
            abort();
    }
}

errno_t make_assembler_code(FILE *const out_stream, Bin_tree_node *const src) {
    assert(out_stream);

    errno_t verify_err = 0;
    CHECK_FUNC(subtree_verify, &verify_err, src);
    if (verify_err) { return verify_err; }

    Translator trans = {};
    CHECK_FUNC(Translator_Ctor, &trans);

    CHECK_FUNC(set_functions_table, &trans, src);

    fprintf_s(out_stream, "JMP ::main\n");
    bool need_pop = false;

    CHECK_FUNC(following_make_assembler_code, out_stream, &need_pop, &trans, src);

    CHECK_FUNC(Translator_Dtor, &trans);

    return 0;
}

#undef FINAL_CODE
