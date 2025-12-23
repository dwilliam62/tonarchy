#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
#include <sys/types.h>

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

#endif
