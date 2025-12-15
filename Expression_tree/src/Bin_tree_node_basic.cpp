#include "Bin_tree_node.h"

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

Bin_tree_node *DSL_new_Bin_tree_node(Bin_tree_node *const left, Bin_tree_node *const right,
                                     Expression_tree_data const data,
                                     errno_t *const err_ptr) {
    assert(err_ptr);

    Bin_tree_node *result = nullptr;
    *err_ptr = My_calloc((void **)&result, 1, sizeof(Bin_tree_node));
    *err_ptr = Bin_tree_node_Ctor(result, left, right, data);
    return result;
}

errno_t Bin_tree_node_Dtor(Bin_tree_node *const node_ptr) {
    assert(node_ptr); assert(node_ptr->is_valid);

    if (node_ptr->data.type == EXPRESSION_TREE_ID_TYPE) { free(node_ptr->data.val.name); }

    node_ptr->is_valid = false;

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

static errno_t subtree_Dtor_uncheked(Bin_tree_node *const node_ptr) {
    if (!node_ptr) { return 0; }

    CHECK_FUNC(subtree_Dtor_uncheked, node_ptr->left);
    CHECK_FUNC(subtree_Dtor_uncheked, node_ptr->right);
    CHECK_FUNC(Bin_tree_node_Dtor, node_ptr);

    return 0;
}

errno_t subtree_Dtor(Bin_tree_node *const node_ptr) {
    errno_t verify_err = 0;
    CHECK_FUNC(Bin_tree_node_verify, &verify_err, node_ptr);
    if (verify_err) { return verify_err; }

    CHECK_FUNC(subtree_Dtor_uncheked, node_ptr);

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
                #include "Text_description/Unary_functions.h"
                #include "Text_description/Binary_functions.h"
                #include "Text_description/Binary_operators.h"
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

static errno_t subtree_following_dot_dump(FILE *const out_stream, Bin_tree_node const *const cur_node) {
    #define LEFT_ARROW_COLOR  "red"
    #define RIGHT_ARROW_COLOR "blue"

    assert(out_stream);

    if (!cur_node) { return 0; }

    CHECK_FUNC(dot_declare_vertex, out_stream, cur_node);

    if (cur_node->left) {
        fprintf_s(out_stream, "\tnode%p:left -> node%p:top"
                              "[color = " LEFT_ARROW_COLOR "]\n",
                              cur_node, cur_node->left);
        CHECK_FUNC(subtree_following_dot_dump, out_stream, cur_node->left);
    }
    if(cur_node->right) {
        fprintf_s(out_stream, "\tnode%p:right -> node%p:top"
                              "[color = " RIGHT_ARROW_COLOR "]\n",
                              cur_node, cur_node->right);
        CHECK_FUNC(subtree_following_dot_dump, out_stream, cur_node->right);
    }

    return 0;

    #undef LEFT_ARROW_COLOR
    #undef RIGHT_ARROW_COLOR
}

errno_t subtree_dot_dump(FILE *const out_stream, Bin_tree_node const *const node_ptr) {
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

#undef FINAL_CODE
