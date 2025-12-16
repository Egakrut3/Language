#include "Bin_tree_node.h"
#include <string.h>

#define FINAL_CODE

errno_t Bin_tree_node_Ctor(Bin_tree_node *const node_ptr,
                           Bin_tree_node *const left, Bin_tree_node *const right,
                           Expression_tree_data const data) {
    assert(node_ptr); assert(!node_ptr->is_valid);

    node_ptr->left        = left;
    node_ptr->right       = right;
    node_ptr->data        = data;
    node_ptr->is_valid    = true;
    node_ptr->verify_used = false;

    return 0;
}

errno_t new_Bin_tree_node(Bin_tree_node **const dest,
                          Bin_tree_node *const left, Bin_tree_node *const right,
                          Expression_tree_data const data) {
    CHECK_FUNC(My_calloc, (void **)dest, 1, sizeof(Bin_tree_node));
    CHECK_FUNC(Bin_tree_node_Ctor, *dest, left, right, data);

    return 0;
}

errno_t Bin_tree_node_Dtor(Bin_tree_node *const node_ptr) {
    assert(node_ptr); assert(node_ptr->is_valid);

    if (node_ptr->data.type == EXPRESSION_TREE_ID_TYPE) { free(node_ptr->data.val.name); }

    node_ptr->is_valid = false;

    return 0;
}

errno_t delete_Bin_tree_node(Bin_tree_node *const dest) {
    assert(dest);

    CHECK_FUNC(Bin_tree_node_Dtor, dest);
    free(dest);

    return 0;
}

errno_t Bin_tree_node_verify(errno_t *const err_ptr, Bin_tree_node const *const node_ptr) {
    assert(node_ptr); assert(err_ptr);

    if (!node_ptr->is_valid)   { *err_ptr |= TREE_NODE_INVALID; }
    if (node_ptr->verify_used) { *err_ptr |= TREE_NODE_VERIFY_USED; }

    return 0;
}

errno_t subtree_verify(errno_t *const err_ptr, Bin_tree_node *const node_ptr) {
    assert(err_ptr);

    if (!node_ptr) { return 0; }

    if (node_ptr->verify_used) { *err_ptr |= TREE_INVALID_STRUCTURE; return 0; }

    CHECK_FUNC(Bin_tree_node_verify, err_ptr, node_ptr);
    node_ptr->verify_used = true;
    CHECK_FUNC(subtree_verify, err_ptr, node_ptr->left);
    CHECK_FUNC(subtree_verify, err_ptr, node_ptr->right);
    node_ptr->verify_used = false;

    return 0;
}

static errno_t delete_subtree_uncheked(Bin_tree_node *const node_ptr) {
    if (!node_ptr) { return 0; }

    CHECK_FUNC(delete_subtree_uncheked, node_ptr->left);
    node_ptr->left = nullptr;
    CHECK_FUNC(delete_subtree_uncheked, node_ptr->right);
    node_ptr->right = nullptr;
    CHECK_FUNC(delete_Bin_tree_node, node_ptr);

    return 0;
}

errno_t delete_subtree(Bin_tree_node *const node_ptr) {
    errno_t verify_err = 0;
    CHECK_FUNC(subtree_verify, &verify_err, node_ptr);
    if (verify_err) { return verify_err; }

    CHECK_FUNC(delete_subtree_uncheked, node_ptr);

    return 0;
}



static uint32_t ptr_hash(void const *const ptr) {
    uint32_t const mlt  = 0X01'00'01'93;
    uint32_t       hash = 0X81'1C'9D'C5;
    for (size_t i = 0; i < sizeof(ptr); ++i) {
        hash = (hash ^ (uint32_t)((size_t)ptr >> i * CHAR_BIT & 0XFF)) * mlt;
    }
    return hash;
}

static uint32_t ptr_color(void const *const ptr) {
    uint32_t hash = ptr_hash(ptr);
    if ((hash       & 0XFF) +
        (hash >> 8  & 0XFF) +
        (hash >> 16 & 0XFF) / 3 < 0X80) {
        hash = ~hash;
    }
    return hash & 0XFF'FF'FF;
}

static errno_t dot_declare_vertex(FILE *const out_stream, Bin_tree_node const *const node_ptr) {
    #define BORDER_COLOR "black"
    #define EMPTY_COLOR  "lightgreen"

    assert(out_stream); assert(node_ptr);

    fprintf_s(out_stream, "\tnode%p [shape = plaintext color = " BORDER_COLOR " style = \"\" "
                                    "label = <<TABLE BORDER=\"0\" CELLBORDER=\"1\" "
                                                    "BGCOLOR=\"#%06X\">"
                                    "<TR><TD COLSPAN=\"2\" PORT=\"top\">[%p]</TD></TR>"
                                    "<TR><TD>is_valid = %d</TD><TD>verify_used = %d</TD></TR>",
                          node_ptr, ptr_color(node_ptr),
                          node_ptr,
                          node_ptr->is_valid, node_ptr->verify_used);

    switch (node_ptr->data.type) {
        case EXPRESSION_TREE_LITERAL_TYPE:
            fprintf_s(out_stream, "<TR><TD COLSPAN=\"2\">type = literal</TD></TR>");
            fprintf_s(out_stream, "<TR><TD COLSPAN=\"2\">val = %lG</TD></TR>", node_ptr->data.val.val);
            break;

        case EXPRESSION_TREE_OPERATION_TYPE:
            fprintf_s(out_stream, "<TR><TD COLSPAN=\"2\">type = operation</TD></TR>");
            switch (node_ptr->data.val.operation) {
                #define HANDLE_OPERATION(name, ...)                                                 \
                case name ## _OPERATION:                                                            \
                    fprintf_s(out_stream, "<TR><TD COLSPAN=\"2\">operation = " #name "</TD></TR>"); \
                    break;
                //This include generates cases for all
                //operations by applying previously declared
                //macros HANDLE_OPERATION to them
                #include "Operation_list.h"
                #undef HANDLE_OPERATION

                default:
                    PRINT_LINE();
                    abort();
            }
            break;

        case EXPRESSION_TREE_ID_TYPE:
            fprintf_s(out_stream, "<TR><TD COLSPAN=\"2\">type = name</TD></TR>");
            fprintf_s(out_stream, "<TR><TD COLSPAN=\"2\">name = %s[%p]</TD></TR>", node_ptr->data.val.name,
                                                                                   node_ptr->data.val.name);
            break;

        default:
            PRINT_LINE();
            abort();
    }

    if (node_ptr->left) {
        fprintf_s(out_stream, "<TR><TD PORT=\"left\" BGCOLOR=\"#%06X\">left = [%p]</TD>",
                              ptr_color(node_ptr->left), node_ptr->left);
    }
    else {
        fprintf_s(out_stream, "<TR><TD PORT=\"left\" BGCOLOR=\"" EMPTY_COLOR "\">left = nil</TD>");
    }
    if (node_ptr->right) {
        fprintf_s(out_stream, "<TD PORT=\"right\" BGCOLOR=\"#%06X\">right = [%p]</TD></TR>",
                              ptr_color(node_ptr->right), node_ptr->right);
    }
    else {
        fprintf_s(out_stream, "<TD PORT=\"right\" BGCOLOR=\"" EMPTY_COLOR "\">right = nil</TD></TR>");
    }

    fprintf_s(out_stream, "</TABLE>>]\n");

    return 0;

    #undef BORDER_COLOR
    #undef EMPTY_COLOR
}

static errno_t subtree_following_dot_dump(FILE *const out_stream, Bin_tree_node *const cur_node) {
    #define LEFT_ARROW_COLOR  "red"
    #define RIGHT_ARROW_COLOR "blue"

    assert(out_stream);

    if (!cur_node) { return 0; }

    CHECK_FUNC(dot_declare_vertex, out_stream, cur_node);

    cur_node->verify_used = true;

    if (cur_node->left) {
        fprintf_s(out_stream, "\tnode%p:left -> node%p:top"
                              "[color = " LEFT_ARROW_COLOR "]\n",
                              cur_node, cur_node->left);
        if (!cur_node->left->verify_used) {
            CHECK_FUNC(subtree_following_dot_dump, out_stream, cur_node->left);
        }
    }
    if(cur_node->right) {
        fprintf_s(out_stream, "\tnode%p:right -> node%p:top"
                              "[color = " RIGHT_ARROW_COLOR "]\n",
                              cur_node, cur_node->right);
        if (!cur_node->right->verify_used) {
            CHECK_FUNC(subtree_following_dot_dump, out_stream, cur_node->right);
        }
    }

    cur_node->verify_used = false;

    return 0;

    #undef LEFT_ARROW_COLOR
    #undef RIGHT_ARROW_COLOR
}

errno_t subtree_dot_dump(FILE *const out_stream, Bin_tree_node *const node_ptr) {
    #define BACKGROUND_COLOR  "white"

    assert(out_stream);

    fprintf_s(out_stream, "digraph {\n");
    fprintf_s(out_stream, "\tnode [shape = octagon style = filled fillcolor = red "
                                  "height = 1.0 width = 1.5]\n");
    fprintf_s(out_stream, "\tedge [color = red penwidth = 3.0]\n");
    fprintf_s(out_stream, "\tbgcolor = \"" BACKGROUND_COLOR "\"\n");
    fprintf_s(out_stream, "\n");

    CHECK_FUNC(subtree_following_dot_dump, out_stream, node_ptr);

    fprintf_s(out_stream, "}");

    return 0;

    #undef BACKGROUND_COLOR
}



errno_t subtree_text_dump(FILE *const out_stream, Bin_tree_node *const src) {
    assert(out_stream);

    if (!src) { fprintf_s(out_stream, "()"); return 0; }

    errno_t verify_err = 0;
    CHECK_FUNC(subtree_verify, &verify_err, src);
    if (verify_err) { return verify_err; }

    fprintf_s(out_stream, "(");

    switch (src->data.type) {
        case EXPRESSION_TREE_LITERAL_TYPE:
            fprintf_s(out_stream, "%lG ", src->data.val.val);
            break;

        case EXPRESSION_TREE_OPERATION_TYPE:
            switch (src->data.val.operation) {
                #define HANDLE_OPERATION(name, ...)     \
                case name ## _OPERATION:                \
                    fprintf_s(out_stream, #name " ");   \
                    break;
                //This include generates cases for all
                //operations by applying previously declared
                //macros HANDLE_OPERATION to them
                #include "Operation_list.h"
                #undef HANDLE_OPERATION

                default:
                    PRINT_LINE();
                    abort();
            }
            break;

        case EXPRESSION_TREE_ID_TYPE:
            fprintf_s(out_stream, "\"%s\" ", src->data.val.name);
            break;

        default:
            PRINT_LINE();
            abort();
    }

    CHECK_FUNC(subtree_text_dump, out_stream, src->left);
    CHECK_FUNC(subtree_text_dump, out_stream, src->right);
    fprintf_s(out_stream, ")");

    return 0;
}

static errno_t skip_spaces(char const **const cur_pos_ptr) {
    assert(cur_pos_ptr); assert(*cur_pos_ptr);

    size_t extra_len = 0;
    CHECK_FUNC(My_sscanf_s, 0, *cur_pos_ptr, " %zn", &extra_len);
    *cur_pos_ptr += extra_len;

    return 0;
}

static errno_t require_character(char const **const cur_pos_ptr, char const c) {
    assert(cur_pos_ptr); assert(*cur_pos_ptr);

    CHECK_FUNC(skip_spaces, cur_pos_ptr);
    if (**cur_pos_ptr != c) { return INCORRECT_TREE_INPUT; }
    *cur_pos_ptr += 1;

    return 0;
}

static bool is_number(char const *const cur_pos) {
    assert(cur_pos);

    char *last_ptr = nullptr;
    strtod(cur_pos, &last_ptr);
    return last_ptr != cur_pos;
}

static errno_t str_prefix_read_subtree_from_pos(Bin_tree_node **const dest,
                                                char const **const cur_pos_ptr) {
    assert(dest); assert(cur_pos_ptr); assert(*cur_pos_ptr);

    size_t extra_len = 0;
    CHECK_FUNC(skip_spaces, cur_pos_ptr);

    if (**cur_pos_ptr == '(') {
        *cur_pos_ptr += 1;

        if (**cur_pos_ptr == ')') { *cur_pos_ptr += 1; *dest = nullptr; return 0; }

        CHECK_FUNC(new_Bin_tree_node, dest, nullptr, nullptr, Expression_tree_data{});

        CHECK_FUNC(skip_spaces, cur_pos_ptr);

        if (is_number(*cur_pos_ptr)) {
            (*dest)->data.type = EXPRESSION_TREE_LITERAL_TYPE;
            CHECK_FUNC(My_sscanf_s, 1, *cur_pos_ptr, "%lG%zn", &(*dest)->data.val.val, &extra_len);
            *cur_pos_ptr += extra_len;
        }
        #define HANDLE_OPERATION(name, ...)                                 \
        else if (!strncmp(*cur_pos_ptr, #name, strlen(#name))) {            \
            (*dest)->data.type          = EXPRESSION_TREE_OPERATION_TYPE;   \
            (*dest)->data.val.operation = name ## _OPERATION;               \
            *cur_pos_ptr += strlen(#name);                                  \
        }
        //This include generates branches of
        //detecting and handling text description
        //of all existing operations by applying
        //previously declared macros HANDLE_OPERATION
        //to them
        #include "Operation_list.h"
        #undef HANDLE_OPERATION
        else if (**cur_pos_ptr == '"') {
            *cur_pos_ptr += 1;

            CHECK_FUNC(My_sscanf_s, 0, *cur_pos_ptr, "%*[^\"]%zn", &extra_len);
            (*dest)->data.type = EXPRESSION_TREE_ID_TYPE;
            CHECK_FUNC(My_calloc, (void **)&(*dest)->data.val.name,
                                  extra_len + 1, sizeof(*(*dest)->data.val.name));
            CHECK_FUNC(strncpy_s, (*dest)->data.val.name, extra_len + 1, *cur_pos_ptr, extra_len);
            *cur_pos_ptr += extra_len;

            CHECK_FUNC(require_character, cur_pos_ptr, '"');
        }
        else { return INCORRECT_TREE_INPUT; }

        CHECK_FUNC(str_prefix_read_subtree_from_pos, &(*dest)->left,  cur_pos_ptr);
        CHECK_FUNC(str_prefix_read_subtree_from_pos, &(*dest)->right, cur_pos_ptr);

        CHECK_FUNC(require_character, cur_pos_ptr, ')');

        return 0;
    }

    return INCORRECT_TREE_INPUT;
}

errno_t str_prefix_read_subtree(Bin_tree_node **const dest, char const *const buffer) {
    char const *cur_pos = buffer;
    CHECK_FUNC(str_prefix_read_subtree_from_pos, dest, &cur_pos);

    return 0;
}

#undef FINAL_CODE
