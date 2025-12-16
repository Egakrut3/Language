#include "Common.h"
#include "Option_manager.h"
#include "Bin_tree_node.h"
//TODO - clear memory
//TODO - verify subtree

int main(int const argc, char const *const *const argv) {
    #define FINAL_CODE

    assert(argc > 0); assert(argv);

    Config cur_config = {};
    MAIN_CHECK_FUNC(Config_Ctor, &cur_config, argc, argv);
    #undef FINAL_CODE
    #define FINAL_CODE          \
    Config_Dtor(&cur_config);

    char *buffer = nullptr;
    MAIN_CHECK_FUNC(get_all_content, cur_config.inp_stream, nullptr, &buffer);
    fclose(cur_config.inp_stream);
    cur_config.inp_stream = nullptr;

    Bin_tree_node *cur_node = nullptr;
    MAIN_CHECK_FUNC(str_prefix_read_subtree, &cur_node, buffer);

    Bin_tree_node *simp_node = nullptr;
    MAIN_CHECK_FUNC(simplify_subtree, &simp_node, cur_node);

    FILE *out_stream = nullptr;
    MAIN_CHECK_FUNC(fopen_s, &out_stream, "./Tree", "w"); //TODO -
    MAIN_CHECK_FUNC(subtree_text_dump, out_stream, simp_node);
    fclose(out_stream);
    out_stream = nullptr;


    MAIN_CHECK_FUNC(fopen_s, &out_stream, "./Logs/dot_file", "w");
    MAIN_CHECK_FUNC(subtree_dot_dump, out_stream, simp_node);
    fclose(out_stream);
    MAIN_CHECK_FUNC(system, "dot -Tsvg ./Logs/dot_file > ./Logs/dot_log.svg");


    colored_printf(GREEN, BLACK, "\n\n\nCOMMIT GITHUB\n\n");
    CLEAR_RESOURCES();
    return 0;

    #undef FINAL_CODE
}
