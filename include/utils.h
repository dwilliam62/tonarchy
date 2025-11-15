#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

bool run_command(const char *cmd);
bool file_exists(const char *path);
void log_info(const char *msg);
void log_error(const char *msg);
char *read_file_list(const char *path);

#endif
