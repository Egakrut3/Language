#include "Common.h"
#include "Option_manager.h"
#include "Bin_tree_node.h"
#include "Name_table.h"

int main(int const argc, char const *const *const argv) {
    #define FINAL_CODE

    assert(argc > 0); assert(argv);

    Config cur_config = {};
    MAIN_CHECK_FUNC(Config_Ctor, &cur_config, argc, argv);
    #undef FINAL_CODE
    #define FINAL_CODE          \
    Config_Dtor(&cur_config);

    Name_table mp = {};
    MAIN_CHECK_FUNC(Name_table_Ctor, &mp);

    MAIN_CHECK_FUNC(Name_table_set, &mp, "abcD", 3);
    MAIN_CHECK_FUNC(Name_table_set, &mp, "def",  6);
    size_t val = 0;
    fprintf_s(stderr, "%zu\n", Name_table_get(&val, &mp, "abcd"));
    MAIN_CHECK_FUNC(Name_table_get, &val, &mp, "abcD");
    fprintf_s(stderr, "%zu\n", val);
    MAIN_CHECK_FUNC(Name_table_get, &val, &mp, "def");
    fprintf_s(stderr, "%zu\n", val);

    //colored_printf(GREEN, BLACK, "\n\n\nCOMMIT GITHUB\n\n");
    CLEAR_RESOURCES();
    return 0;

    #undef FINAL_CODE
}
