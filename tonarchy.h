#ifndef TONARCHY_H
#define TONARCHY_H

#define _POSIX_C_SOURCE 200809L

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctype.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

#define CHROOT_PATH "/mnt"
#define MAX_CMD_SIZE 4096

typedef enum {
    LEVEL_BEGINNER,
    LEVEL_INTERMEDIATE,
    LEVEL_ADVANCED
} install_level;

typedef enum {
    DE_CINNAMON,
    DE_HYPRLAND,
    DE_GNOME,
    DE_KDE,
    DE_SWAY,
    DE_NONE
} desktop_env;

typedef enum {
    DISPLAY_WAYLAND,
    DISPLAY_XORG,
    DISPLAY_BOTH
} display_server;

typedef enum {
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR
} LogLevel;

typedef struct {
    install_level level;
    desktop_env de;
    display_server display;
    bool use_custom_dotfiles;
    char dotfiles_url[512];
    bool install_docker;
    bool install_dev_tools;
    bool install_gaming;
} install_config;

typedef struct {
    const char *repo_url;
    const char *name;
    const char *build_dir;
} GitRepo;

typedef struct {
    const char *filename;
    const char *content;
    mode_t permissions;
} DotFile;

typedef struct {
    const char *key;
    const char *value;
} ConfigEntry;

typedef struct {
    const char *service_name;
    const char *drop_in_dir;
    const char *drop_in_file;
    ConfigEntry *entries;
    size_t entry_count;
} SystemdOverride;

typedef struct {
    char *cinnamon_package;
    char *suckless_package;
} Packages;

void logger_init(const char *log_path);
void logger_close(void);
void log_msg(LogLevel level, const char *fmt, ...);

#define LOG_DEBUG(...) log_msg(LOG_LEVEL_DEBUG, __VA_ARGS__)
#define LOG_INFO(...)  log_msg(LOG_LEVEL_INFO, __VA_ARGS__)
#define LOG_WARN(...)  log_msg(LOG_LEVEL_WARN, __VA_ARGS__)
#define LOG_ERROR(...) log_msg(LOG_LEVEL_ERROR, __VA_ARGS__)

int write_file(const char *path, const char *content);
int write_file_fmt(const char *path, const char *fmt, ...);
int set_file_perms(const char *path, mode_t mode, const char *owner, const char *group);
int create_directory(const char *path, mode_t mode);
int chroot_exec(const char *cmd);
int chroot_exec_fmt(const char *fmt, ...);
int chroot_exec_as_user(const char *username, const char *cmd);
int chroot_exec_as_user_fmt(const char *username, const char *fmt, ...);
int git_clone_as_user(const char *username, const char *repo_url, const char *dest_path);
int make_clean_install(const char *build_dir);
int create_user_dotfile(const char *username, const DotFile *dotfile);
int setup_systemd_override(const SystemdOverride *override);

void show_message(const char *message);

#define CHECK_OR_FAIL(expr, user_msg) \
    do { \
        if (!(expr)) { \
            LOG_ERROR("%s", #expr); \
            show_message(user_msg); \
            return 0; \
        } \
    } while(0)

#endif
