#include "Code_reading.h"
#include <string.h>

#define FINAL_CODE

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

enum Expression_token_type {
    TOKEN_LITERAL_TYPE,
    TOKEN_KEYWORD_TYPE,
    TOKEN_ID_TYPE,
};

union Expression_token_val {
    double                val;
    Expression_token_type type;
    char                  *name;
};

struct Expression_token {
    Expression_token_type type;
    Expression_token_val  val;
};

//TODO - use tokenizator

static errno_t read_number(Bin_tree_node **const dest, char const **const cur_pos_ptr) {
    assert(dest); assert(cur_pos_ptr); assert(*cur_pos_ptr);

    size_t extra_len = 0;

    CHECK_FUNC(new_Bin_tree_node, dest, nullptr, nullptr,
                                  Expression_tree_data{EXPRESSION_TREE_LITERAL_TYPE,
                                                       Expression_tree_node_val{}});

    CHECK_FUNC(My_sscanf_s, 1, *cur_pos_ptr, "%lG%zn", &(*dest)->data.val.val, &extra_len);
    *cur_pos_ptr += extra_len;

    return 0;
}

static errno_t read_ID(Bin_tree_node **const dest, char const **cur_pos_ptr) {
    assert(dest); assert(cur_pos_ptr); assert(*cur_pos_ptr);

    size_t extra_len = 0;

    CHECK_FUNC(new_Bin_tree_node, dest, nullptr, nullptr,
                                  Expression_tree_data{EXPRESSION_TREE_ID_TYPE,
                                                       Expression_tree_node_val{}});

    CHECK_FUNC(skip_spaces, cur_pos_ptr);
    CHECK_FUNC(My_sscanf_s, 1, *cur_pos_ptr, "%*[^ \f\n\r\t\v+-*/^,)$]%zn", &extra_len);
    CHECK_FUNC(My_calloc, (void **)&(*dest)->data.val.name, extra_len + 1, sizeof(*(*dest)->data.val.name));
    CHECK_FUNC(strncpy_s, (*dest)->data.val.name, extra_len + 1, *cur_pos_ptr, extra_len);
    *cur_pos_ptr += extra_len;

    return 0;
}

static errno_t read_S1(Bin_tree_node **dest, char const **cur_pos_ptr);

static errno_t read_primary(Bin_tree_node **const dest, char const **const cur_pos_ptr) {
    assert(dest); assert(cur_pos_ptr); assert(*cur_pos_ptr);

    CHECK_FUNC(skip_spaces, cur_pos_ptr);

    if (**cur_pos_ptr == '(') {
        *cur_pos_ptr += 1;

        CHECK_FUNC(read_S1, dest, cur_pos_ptr);

        CHECK_FUNC(require_character, cur_pos_ptr, ')');

        return 0;
    }

    if (is_number(*cur_pos_ptr)) { CHECK_FUNC(read_number, dest, cur_pos_ptr); return 0; }

    #define HANDLE_OPERATION(name, text_description, ...)                                           \
    if (!strncmp(*cur_pos_ptr, text_description, strlen(text_description))) {                       \
        *cur_pos_ptr += strlen(text_description);                                                   \
                                                                                                    \
        CHECK_FUNC(new_Bin_tree_node, dest, nullptr, nullptr,                                       \
                                      Expression_tree_data{                                         \
                                      EXPRESSION_TREE_OPERATION_TYPE,                               \
                                      Expression_tree_node_val{.operation = name ## _OPERATION}});  \
                                                                                                    \
        CHECK_FUNC(require_character, cur_pos_ptr, '(');                                            \
                                                                                                    \
        CHECK_FUNC(read_S1, &(*dest)->right, cur_pos_ptr);                                          \
                                                                                                    \
        CHECK_FUNC(require_character, cur_pos_ptr, ')');                                            \
                                                                                                    \
        return 0;                                                                                   \
    }
    //This include generates branches of
    //detecting and handling text description
    //of unary functions by applying
    //previously declared macros HANDLE_OPERATION
    //to them
    #include "Text_description/Unary_functions.h"
    #undef HANDLE_OPERATION

    #define HANDLE_OPERATION(name, text_description, ...)                                           \
    if (!strncmp(*cur_pos_ptr, text_description, strlen(text_description))) {                       \
        *cur_pos_ptr += strlen(text_description);                                                   \
                                                                                                    \
        CHECK_FUNC(new_Bin_tree_node, dest, nullptr, nullptr,                                       \
                                      Expression_tree_data{                                         \
                                      EXPRESSION_TREE_OPERATION_TYPE,                               \
                                      Expression_tree_node_val{.operation = name ## _OPERATION}});  \
                                                                                                    \
        CHECK_FUNC(require_character, cur_pos_ptr, '(');                                            \
                                                                                                    \
        CHECK_FUNC(read_S1, &(*dest)->left,  cur_pos_ptr);                                          \
                                                                                                    \
        CHECK_FUNC(require_character, cur_pos_ptr, ',');                                            \
                                                                                                    \
        CHECK_FUNC(read_S1, &(*dest)->right, cur_pos_ptr);                                          \
                                                                                                    \
        CHECK_FUNC(require_character, cur_pos_ptr, ')');                                            \
                                                                                                    \
        return 0;                                                                                   \
    }
    //This include generates branches of
    //detecting and handling text description
    //of binary functions by applying
    //previously declared macros HANDLE_OPERATION
    //to them
    #include "Text_description/Binary_functions.h"
    #undef HANDLE_OPERATION

    CHECK_FUNC(read_ID, dest, cur_pos_ptr);

    return 0;
}

static errno_t read_S3(Bin_tree_node **const dest, char const **const cur_pos_ptr) {
    assert(dest); assert(cur_pos_ptr); assert(*cur_pos_ptr);

    CHECK_FUNC(read_primary, dest, cur_pos_ptr);

    CHECK_FUNC(skip_spaces, cur_pos_ptr);
    if (**cur_pos_ptr == '^') {
        *cur_pos_ptr += 1;

        CHECK_FUNC(new_Bin_tree_node, dest, *dest, nullptr,
                                      Expression_tree_data{
                                      EXPRESSION_TREE_OPERATION_TYPE,
                                      Expression_tree_node_val{.operation = POW_OPERATION}});

        CHECK_FUNC(read_S3, &(*dest)->right, cur_pos_ptr);

    }

    return 0;
}

static errno_t read_S2(Bin_tree_node **const dest, char const **const cur_pos_ptr) {
    assert(dest); assert(cur_pos_ptr); assert(*cur_pos_ptr);

    CHECK_FUNC(read_S3, dest, cur_pos_ptr);

    while (true) {
        CHECK_FUNC(skip_spaces, cur_pos_ptr);
        if (**cur_pos_ptr == '*') {
            *cur_pos_ptr += 1;

            CHECK_FUNC(new_Bin_tree_node, dest, *dest, nullptr,
                                          Expression_tree_data{
                                          EXPRESSION_TREE_OPERATION_TYPE,
                                          Expression_tree_node_val{.operation = MLT_OPERATION}});

            CHECK_FUNC(read_S3, &(*dest)->right, cur_pos_ptr);
        }
        else if (**cur_pos_ptr == '/') {
            *cur_pos_ptr += 1;

            CHECK_FUNC(new_Bin_tree_node, dest, *dest, nullptr,
                                          Expression_tree_data{
                                          EXPRESSION_TREE_OPERATION_TYPE,
                                          Expression_tree_node_val{.operation = DIV_OPERATION}});

            CHECK_FUNC(read_S3, &(*dest)->right, cur_pos_ptr);
        }
        else { return 0; }
    }

    PRINT_LINE();
    abort();
}

static errno_t read_S1(Bin_tree_node **const dest, char const **const cur_pos_ptr) {
    assert(dest); assert(cur_pos_ptr); assert(*cur_pos_ptr);

    CHECK_FUNC(read_S2, dest, cur_pos_ptr);

    while (true) {
        CHECK_FUNC(skip_spaces, cur_pos_ptr);
        if (**cur_pos_ptr == '+') {
            *cur_pos_ptr += 1;

            CHECK_FUNC(new_Bin_tree_node, dest, *dest, nullptr,
                                          Expression_tree_data{
                                          EXPRESSION_TREE_OPERATION_TYPE,
                                          Expression_tree_node_val{.operation = ADD_OPERATION}});

            CHECK_FUNC(read_S2, &(*dest)->right, cur_pos_ptr);
        }
        else if (**cur_pos_ptr == '-') {
            *cur_pos_ptr += 1;

            CHECK_FUNC(new_Bin_tree_node, dest, *dest, nullptr,
                                          Expression_tree_data{
                                          EXPRESSION_TREE_OPERATION_TYPE,
                                          Expression_tree_node_val{.operation = SUB_OPERATION}});

            CHECK_FUNC(read_S2, &(*dest)->right, cur_pos_ptr);
        }
        else { return 0; }
    }

    PRINT_LINE();
    abort();
}

static errno_t read_expression(Bin_tree_node **const dest, char const **const cur_pos_ptr) {
    assert(cur_pos_ptr); assert(*cur_pos_ptr);

    CHECK_FUNC(read_S1, dest, cur_pos_ptr);

    CHECK_FUNC(require_character, cur_pos_ptr, '$');

    return 0;
}

errno_t read_code(Bin_tree_node **const dest, char const *const buffer) {
    char const *cur_pos_ptr = buffer;
    CHECK_FUNC(read_expression, dest, &cur_pos_ptr);

    return 0;
}

#undef FINAL_CODE
