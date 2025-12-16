#include "Common.h"
#include "Option_manager.h"
#include "Bin_tree_node.h"

int main(int const argc, char const *const *const argv) {
    #define FINAL_CODE

    assert(argc > 0); assert(argv);

    Config cur_config = {};
    MAIN_CHECK_FUNC(Config_Ctor, &cur_config, argc, argv);
    #undef FINAL_CODE
    #define FINAL_CODE          \
    Config_Dtor(&cur_config);

    FILE *inp_stream = nullptr;
    fopen_s(&inp_stream, "./Tree_in", "r");
    char *buffer = nullptr;
    MAIN_CHECK_FUNC(get_all_content, inp_stream, nullptr, &buffer);
    fclose(inp_stream);


    Bin_tree_node *cur_node = nullptr;

    MAIN_CHECK_FUNC(str_infix_read_subtree, &cur_node, buffer);

    /*
    MAIN_CHECK_FUNC(str_prefix_read_subtree, &cur_node, buffer);
    */


    FILE *out_stream = nullptr;
    MAIN_CHECK_FUNC(fopen_s, &out_stream, "./Logs/dot_file", "w");
    MAIN_CHECK_FUNC(subtree_dot_dump, out_stream, cur_node);
    fclose(out_stream);
    MAIN_CHECK_FUNC(system, "dot -Tsvg ./Logs/dot_file > ./Logs/dot_log.svg");


    /*
    MAIN_CHECK_FUNC(fopen_s, &out_stream, "./Logs/Tree_out", "w");
    MAIN_CHECK_FUNC(subtree_text_dump, out_stream, cur_node);
    fclose(out_stream);
    */

    colored_printf(GREEN, BLACK, "\n\n\nCOMMIT GITHUB\n\n");
    CLEAR_RESOURCES();
    return 0;

    #undef FINAL_CODE
}
