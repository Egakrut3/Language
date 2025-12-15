#ifndef OPTION_MANAGER_H
#define OPTION_MANAGER_H

#include "Common.h"

#define UNKNOWN_OPTION              500
#define NOT_ENOUGH_OPTION_ARGUMENTS 501

struct Config
{
    FILE *inp_stream;
    bool is_help;

    bool is_valid;
};

errno_t Config_Ctor(Config *config_ptr, size_t argc, char const *const *argv);

errno_t Config_Dtor(Config *config_ptr);

#endif
