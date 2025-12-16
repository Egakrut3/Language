#include "Common.h"
#include "Code_reading.h"
#include "Option_manager.h"

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
    MAIN_CHECK_FUNC(read_code, &cur_node, buffer);

    FILE *out_stream = nullptr;
    MAIN_CHECK_FUNC(fopen_s, &out_stream, "./Logs/dot_file", "w");
    MAIN_CHECK_FUNC(subtree_dot_dump, out_stream, cur_node);
    fclose(out_stream);
    out_stream = nullptr;
    MAIN_CHECK_FUNC(system, "dot -Tsvg ./Logs/dot_file > ./Logs/dot_log.svg");

    MAIN_CHECK_FUNC(subtree_text_dump, cur_config.out_stream, cur_node);
    fclose(cur_config.out_stream);
    cur_config.out_stream = nullptr;


    MAIN_CHECK_FUNC(delete_Bin_tree_node, cur_node);
    cur_node = nullptr;

    //colored_printf(GREEN, BLACK, "\n\n\nCOMMIT GITHUB\n\n");
    CLEAR_RESOURCES();
    return 0;

    #undef FINAL_CODE
}
