#ifndef INCLUDE_FILE_INPUT_H
#define INCLUDE_FILE_INPUT_H

#include "Common_declarations.h"

errno_t get_path_filesize(char const *path, __int64 *filesize);
errno_t get_opened_filesize(FILE *cur_file, __int64 *filesize);

errno_t clear_file(char const *path);
errno_t get_all_content(FILE *stream, size_t *filesize_dest, char **buffer);

#endif /* INCLUDE_FILE_INPUT_H */
