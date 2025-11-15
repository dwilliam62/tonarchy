#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <ctype.h>

static struct termios orig_termios;

static void disable_raw_mode(void) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

static void enable_raw_mode(void) {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disable_raw_mode);

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

static void get_terminal_size(int *rows, int *cols) {
    struct winsize ws;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
    *rows = ws.ws_row;
    *cols = ws.ws_col;
}

static void clear_screen(void) {
    printf("\033[2J\033[H");
    fflush(stdout);
}

static void draw_logo(int cols) {
    const char *logo[] = {
        "████████╗ ██████╗ ███╗   ██╗ █████╗ ██████╗  ██████╗██╗  ██╗██╗   ██╗",
        "╚══██╔══╝██╔═══██╗████╗  ██║██╔══██╗██╔══██╗██╔════╝██║  ██║╚██╗ ██╔╝",
        "   ██║   ██║   ██║██╔██╗ ██║███████║██████╔╝██║     ███████║ ╚████╔╝ ",
        "   ██║   ██║   ██║██║╚██╗██║██╔══██║██╔══██╗██║     ██╔══██║  ╚██╔╝  ",
        "   ██║   ╚██████╔╝██║ ╚████║██║  ██║██║  ██║╚██████╗██║  ██║   ██║   ",
        "   ╚═╝    ╚═════╝ ╚═╝  ╚═══╝╚═╝  ╚═╝╚═╝  ╚═╝ ╚═════╝╚═╝  ╚═╝   ╚═╝   "
    };

    int logo_height = 6;
    int logo_start = (cols - 70) / 2;

    printf("\033[1;32m");
    for (int i = 0; i < logo_height; i++) {
        printf("\033[%d;%dH%s", i + 2, logo_start, logo[i]);
    }
    printf("\033[0m");
}

static int draw_menu(const char **items, int count, int selected) {
    int rows, cols;
    get_terminal_size(&rows, &cols);

    clear_screen();
    draw_logo(cols);

    int logo_start = (cols - 70) / 2;
    int menu_start_row = 10;

    for (int i = 0; i < count; i++) {
        printf("\033[%d;%dH", menu_start_row + i, logo_start + 2);
        if (i == selected) {
            printf("\033[1;34m> %s\033[0m", items[i]);
        } else {
            printf("\033[37m  %s\033[0m", items[i]);
        }
    }

    printf("\033[%d;%dH", menu_start_row + count + 2, logo_start);
    printf("\033[33mj/k Navigate  Enter Select\033[0m");

    fflush(stdout);
    return 0;
}

static int select_from_menu(const char **items, int count) {
    int selected = 0;

    enable_raw_mode();
    draw_menu(items, count, selected);

    char c;
    while (read(STDIN_FILENO, &c, 1) == 1) {
        if (c == 'q' || c == 27) {
            disable_raw_mode();
            return -1;
        }

        if (c == 'j' || c == 66) {
            if (selected < count - 1) {
                selected++;
                draw_menu(items, count, selected);
            }
        }

        if (c == 'k' || c == 65) {
            if (selected > 0) {
                selected--;
                draw_menu(items, count, selected);
            }
        }

        if (c == '\r' || c == '\n') {
            disable_raw_mode();
            return selected;
        }
    }

    disable_raw_mode();
    return -1;
}

static void show_message(const char *message) {
    int rows, cols;
    get_terminal_size(&rows, &cols);

    clear_screen();
    draw_logo(cols);

    int logo_start = (cols - 70) / 2;
    printf("\033[%d;%dH", 10, logo_start);
    printf("\033[37m%s\033[0m", message);
    fflush(stdout);

    sleep(2);
}

static void draw_form(
        const char *username,
        const char *password,
        const char *confirmed_password,
        const char *hostname,
        const char *keyboard,
        const char *timezone,
        int current_field
    ) {
    int rows, cols;
    get_terminal_size(&rows, &cols);

    clear_screen();
    draw_logo(cols);

    int logo_start = (cols - 70) / 2;
    int form_row = 10;

    printf("\033[%d;%dH", form_row, logo_start);
    printf("\033[37mSetup your system:\033[0m");

    form_row += 2;

    if (current_field == 0) {
        printf("\033[%d;%dH\033[1;34m>\033[0m ", form_row, logo_start);
    } else {
        printf("\033[%d;%dH  ", form_row, logo_start);
    }
    printf("\033[37mUsername: \033[0m");
    if (strlen(username) > 0) {
        printf("\033[32m%s\033[0m", username);
    } else if (current_field != 0) {
        printf("\033[90m[not set]\033[0m");
    }

    form_row++;

    if (current_field == 1) {
        printf("\033[%d;%dH\033[1;34m>\033[0m ", form_row, logo_start);
    } else {
        printf("\033[%d;%dH  ", form_row, logo_start);
    }
    printf("\033[37mPassword: \033[0m");
    if (strlen(password) > 0) {
        printf("\033[32m%s\033[0m", "********");
    } else if (current_field != 1) {
        printf("\033[90m[not set]\033[0m");
    }

    form_row++;

    if (current_field == 2) {
        printf("\033[%d;%dH\033[1;34m>\033[0m ", form_row, logo_start);
    } else {
        printf("\033[%d;%dH  ", form_row, logo_start);
    }
    printf("\033[37mConfirm Password: \033[0m");
    if (strlen(confirmed_password) > 0) {
        printf("\033[32m%s\033[0m", "********");
    } else if (current_field != 2) {
        printf("\033[90m[not set]\033[0m");
    }

    form_row++;

    if (current_field == 3) {
        printf("\033[%d;%dH\033[1;34m>\033[0m ", form_row, logo_start);
    } else {
        printf("\033[%d;%dH  ", form_row, logo_start);
    }
    printf("\033[37mHostname: \033[0m");
    if (strlen(hostname) > 0) {
        printf("\033[32m%s\033[0m", hostname);
    } else if (current_field != 3) {
        printf("\033[90mtonarchy\033[0m");
    }

    form_row++;

    if (current_field == 4) {
        printf("\033[%d;%dH\033[1;34m>\033[0m ", form_row, logo_start);
    } else {
        printf("\033[%d;%dH  ", form_row, logo_start);
    }
    printf("\033[37mKeyboard: \033[0m");
    if (strlen(keyboard) > 0) {
        printf("\033[32m%s\033[0m", keyboard);
    } else if (current_field != 4) {
        printf("\033[90mus\033[0m");
    }

    form_row++;

    if (current_field == 5) {
        printf("\033[%d;%dH\033[1;34m>\033[0m ", form_row, logo_start);
    } else {
        printf("\033[%d;%dH  ", form_row, logo_start);
    }
    printf("\033[37mTimezone: \033[0m");
    if (strlen(timezone) > 0) {
        printf("\033[32m%s\033[0m", timezone);
    } else if (current_field != 5) {
        printf("\033[90m[not set]\033[0m");
    }

    fflush(stdout);
}

static int get_form_input(
        char *username,
        char *password,
        char *confirmed_password,
        char *hostname,
        char *keyboard,
        char *timezone
    ) {
    char temp_input[256];
    char password_confirm[256];
    int current_field = 0;
    int rows, cols;
    get_terminal_size(&rows, &cols);
    int logo_start = (cols - 70) / 2;
    int form_row = 12;

    while (current_field < 6) {
        draw_form(username, password, confirmed_password, hostname, keyboard, timezone, current_field);

        if (current_field == 0) {
            printf("\033[%d;%dH", form_row, logo_start + 13);
            fflush(stdout);

            struct termios old_term;
            tcgetattr(STDIN_FILENO, &old_term);
            struct termios new_term = old_term;
            new_term.c_lflag |= (ECHO | ICANON);
            new_term.c_lflag &= ~ISIG;
            tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_term);

            if (fgets(temp_input, sizeof(temp_input), stdin) == NULL) {
                tcsetattr(STDIN_FILENO, TCSAFLUSH, &old_term);
                return 0;
            }
            temp_input[strcspn(temp_input, "\n")] = '\0';
            tcsetattr(STDIN_FILENO, TCSAFLUSH, &old_term);

            if (strlen(temp_input) > 0) {
                int valid = 1;
                for (int i = 0; temp_input[i]; i++) {
                    if (!isalnum(temp_input[i]) && temp_input[i] != '-' && temp_input[i] != '_') {
                        valid = 0;
                        break;
                    }
                }
                if (valid) {
                    strcpy(username, temp_input);
                    current_field++;
                } else {
                    show_message("Username must be alphanumeric");
                }
            }
        } else if (current_field == 1) {
            printf("\033[%d;%dH", form_row + 1, logo_start + 13);
            fflush(stdout);

            struct termios old_term;
            tcgetattr(STDIN_FILENO, &old_term);
            struct termios new_term = old_term;
            new_term.c_lflag &= ~ECHO;
            new_term.c_lflag |= ICANON;
            new_term.c_lflag &= ~ISIG;
            tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_term);

            if (fgets(temp_input, sizeof(temp_input), stdin) == NULL) {
                tcsetattr(STDIN_FILENO, TCSAFLUSH, &old_term);
                return 0;
            }
            temp_input[strcspn(temp_input, "\n")] = '\0';
            tcsetattr(STDIN_FILENO, TCSAFLUSH, &old_term);

            if (strlen(temp_input) == 0) {
                show_message("Password cannot be empty");
                continue;
            }

            strcpy(password, temp_input);
            current_field++;
        } else if (current_field == 2) {
            printf("\033[%d;%dH", form_row + 2, logo_start + 20);
            fflush(stdout);

            struct termios old_term;
            tcgetattr(STDIN_FILENO, &old_term);
            struct termios new_term = old_term;
            new_term.c_lflag &= ~ECHO;
            new_term.c_lflag |= ICANON;
            new_term.c_lflag &= ~ISIG;
            tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_term);

            if (fgets(password_confirm, sizeof(password_confirm), stdin) == NULL) {
                tcsetattr(STDIN_FILENO, TCSAFLUSH, &old_term);
                return 0;
            }
            password_confirm[strcspn(password_confirm, "\n")] = '\0';
            tcsetattr(STDIN_FILENO, TCSAFLUSH, &old_term);

            if (strcmp(password, password_confirm) == 0) {
                strcpy(confirmed_password, password_confirm);
                current_field++;
            } else {
                show_message("Passwords do not match");
            }
        } else if (current_field == 3) {
            printf("\033[%d;%dH", form_row + 3, logo_start + 13);
            fflush(stdout);

            struct termios old_term;
            tcgetattr(STDIN_FILENO, &old_term);
            struct termios new_term = old_term;
            new_term.c_lflag |= (ECHO | ICANON);
            new_term.c_lflag &= ~ISIG;
            tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_term);

            if (fgets(temp_input, sizeof(temp_input), stdin) == NULL) {
                tcsetattr(STDIN_FILENO, TCSAFLUSH, &old_term);
                return 0;
            }
            temp_input[strcspn(temp_input, "\n")] = '\0';
            tcsetattr(STDIN_FILENO, TCSAFLUSH, &old_term);

            if (strlen(temp_input) == 0) {
                strcpy(hostname, "tonarchy");
            } else {
                int valid = 1;
                for (int i = 0; temp_input[i]; i++) {
                    if (!isalnum(temp_input[i]) && temp_input[i] != '-' && temp_input[i] != '_') {
                        valid = 0;
                        break;
                    }
                }
                if (valid) {
                    strcpy(hostname, temp_input);
                } else {
                    show_message("Hostname must be alphanumeric");
                    continue;
                }
            }
            current_field++;
        } else if (current_field == 4) {
            clear_screen();
            FILE *fp = popen("localectl list-keymaps | fzf --height=40% --reverse --prompt='Keyboard: ' --header='Start typing to filter, Enter to select' --query='us'", "r");
            if (fp == NULL) {
                show_message("Failed to open keyboard selector");
                continue;
            }

            if (fgets(temp_input, sizeof(temp_input), fp) != NULL) {
                temp_input[strcspn(temp_input, "\n")] = '\0';
                if (strlen(temp_input) > 0) {
                    strcpy(keyboard, temp_input);
                }
            }
            pclose(fp);

            if (strlen(keyboard) == 0) {
                strcpy(keyboard, "us");
            }
            current_field++;
        } else if (current_field == 5) {
            clear_screen();
            FILE *fp = popen("timedatectl list-timezones | fzf --height=40% --reverse --prompt='Timezone: ' --header='Type your city/timezone, Enter to select'", "r");
            if (fp == NULL) {
                show_message("Failed to open timezone selector");
                continue;
            }

            if (fgets(temp_input, sizeof(temp_input), fp) != NULL) {
                temp_input[strcspn(temp_input, "\n")] = '\0';
                if (strlen(temp_input) > 0) {
                    strcpy(timezone, temp_input);
                }
            }
            pclose(fp);

            if (strlen(timezone) == 0) {
                show_message("Timezone is required");
            } else {
                current_field++;
            }
        }
    }

    while (1) {
        draw_form(username, password, confirmed_password, hostname, keyboard, timezone, 6);

        int rows, cols;
        get_terminal_size(&rows, &cols);
        int logo_start = (cols - 70) / 2;

        printf("\033[%d;%dH\033[33mPress Enter to continue, or field number to edit (0-5)\033[0m", 20, logo_start);
        fflush(stdout);

        enable_raw_mode();
        char c;
        if (read(STDIN_FILENO, &c, 1) == 1) {
            if (c == '\r' || c == '\n') {
                disable_raw_mode();
                return 1;
            }
            if (c == 'q' || c == 27) {
                disable_raw_mode();
                return 0;
            }
            if (c >= '0' && c <= '5') {
                disable_raw_mode();
                int edit_field = c - '0';

                if (edit_field == 0) {
                    draw_form(username, password, confirmed_password, hostname, keyboard, timezone, 0);
                    printf("\033[%d;%dH", form_row, logo_start + 13);
                    fflush(stdout);

                    struct termios old_term;
                    tcgetattr(STDIN_FILENO, &old_term);
                    struct termios new_term = old_term;
                    new_term.c_lflag |= (ECHO | ICANON);
                    new_term.c_lflag &= ~ISIG;
                    tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_term);

                    if (fgets(temp_input, sizeof(temp_input), stdin) != NULL) {
                        temp_input[strcspn(temp_input, "\n")] = '\0';
                        if (strlen(temp_input) > 0) {
                            int valid = 1;
                            for (int i = 0; temp_input[i]; i++) {
                                if (!isalnum(temp_input[i]) && temp_input[i] != '-' && temp_input[i] != '_') {
                                    valid = 0;
                                    break;
                                }
                            }
                            if (valid) {
                                strcpy(username, temp_input);
                            } else {
                                show_message("Username must be alphanumeric");
                            }
                        }
                    }
                    tcsetattr(STDIN_FILENO, TCSAFLUSH, &old_term);
                } else if (edit_field == 1) {
                    draw_form(username, password, confirmed_password, hostname, keyboard, timezone, 1);
                    printf("\033[%d;%dH", form_row + 1, logo_start + 13);
                    fflush(stdout);

                    struct termios old_term;
                    tcgetattr(STDIN_FILENO, &old_term);
                    struct termios new_term = old_term;
                    new_term.c_lflag &= ~ECHO;
                    new_term.c_lflag |= ICANON;
                    new_term.c_lflag &= ~ISIG;
                    tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_term);

                    if (fgets(temp_input, sizeof(temp_input), stdin) != NULL) {
                        temp_input[strcspn(temp_input, "\n")] = '\0';
                        if (strlen(temp_input) > 0) {
                            strcpy(password, temp_input);

                            draw_form(username, password, confirmed_password, hostname, keyboard, timezone, 2);
                            printf("\033[%d;%dH", form_row + 2, logo_start + 20);
                            fflush(stdout);

                            if (fgets(password_confirm, sizeof(password_confirm), stdin) != NULL) {
                                password_confirm[strcspn(password_confirm, "\n")] = '\0';
                                if (strcmp(password, password_confirm) == 0) {
                                    strcpy(confirmed_password, password_confirm);
                                } else {
                                    show_message("Passwords do not match");
                                }
                            }
                        }
                    }
                    tcsetattr(STDIN_FILENO, TCSAFLUSH, &old_term);
                } else if (edit_field == 2) {
                    draw_form(username, password, confirmed_password, hostname, keyboard, timezone, 1);
                    printf("\033[%d;%dH", form_row + 1, logo_start + 13);
                    fflush(stdout);

                    struct termios old_term;
                    tcgetattr(STDIN_FILENO, &old_term);
                    struct termios new_term = old_term;
                    new_term.c_lflag &= ~ECHO;
                    new_term.c_lflag |= ICANON;
                    new_term.c_lflag &= ~ISIG;
                    tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_term);

                    if (fgets(temp_input, sizeof(temp_input), stdin) != NULL) {
                        temp_input[strcspn(temp_input, "\n")] = '\0';
                        if (strlen(temp_input) > 0) {
                            strcpy(password, temp_input);

                            draw_form(username, password, confirmed_password, hostname, keyboard, timezone, 2);
                            printf("\033[%d;%dH", form_row + 2, logo_start + 20);
                            fflush(stdout);

                            if (fgets(password_confirm, sizeof(password_confirm), stdin) != NULL) {
                                password_confirm[strcspn(password_confirm, "\n")] = '\0';
                                if (strcmp(password, password_confirm) == 0) {
                                    strcpy(confirmed_password, password_confirm);
                                } else {
                                    show_message("Passwords do not match");
                                }
                            }
                        }
                    }
                    tcsetattr(STDIN_FILENO, TCSAFLUSH, &old_term);
                } else if (edit_field == 3) {
                    draw_form(username, password, confirmed_password, hostname, keyboard, timezone, 3);
                    printf("\033[%d;%dH", form_row + 3, logo_start + 13);
                    fflush(stdout);

                    struct termios old_term;
                    tcgetattr(STDIN_FILENO, &old_term);
                    struct termios new_term = old_term;
                    new_term.c_lflag |= (ECHO | ICANON);
                    new_term.c_lflag &= ~ISIG;
                    tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_term);

                    if (fgets(temp_input, sizeof(temp_input), stdin) != NULL) {
                        temp_input[strcspn(temp_input, "\n")] = '\0';
                        if (strlen(temp_input) == 0) {
                            strcpy(hostname, "tonarchy");
                        } else {
                            int valid = 1;
                            for (int i = 0; temp_input[i]; i++) {
                                if (!isalnum(temp_input[i]) && temp_input[i] != '-' && temp_input[i] != '_') {
                                    valid = 0;
                                    break;
                                }
                            }
                            if (valid) {
                                strcpy(hostname, temp_input);
                            } else {
                                show_message("Hostname must be alphanumeric");
                            }
                        }
                    }
                    tcsetattr(STDIN_FILENO, TCSAFLUSH, &old_term);
                } else if (edit_field == 4) {
                    clear_screen();
                    FILE *fp = popen("localectl list-keymaps | fzf --height=40% --reverse --prompt='Keyboard: ' --header='Start typing to filter, Enter to select' --query='us'", "r");
                    if (fp != NULL) {
                        if (fgets(temp_input, sizeof(temp_input), fp) != NULL) {
                            temp_input[strcspn(temp_input, "\n")] = '\0';
                            if (strlen(temp_input) > 0) {
                                strcpy(keyboard, temp_input);
                            }
                        }
                        pclose(fp);
                    }
                    if (strlen(keyboard) == 0) {
                        strcpy(keyboard, "us");
                    }
                } else if (edit_field == 5) {
                    clear_screen();
                    FILE *fp = popen("timedatectl list-timezones | fzf --height=40% --reverse --prompt='Timezone: ' --header='Type your city/timezone, Enter to select'", "r");
                    if (fp != NULL) {
                        if (fgets(temp_input, sizeof(temp_input), fp) != NULL) {
                            temp_input[strcspn(temp_input, "\n")] = '\0';
                            if (strlen(temp_input) > 0) {
                                strcpy(timezone, temp_input);
                            }
                        }
                        pclose(fp);
                    }
                    if (strlen(timezone) == 0) {
                        show_message("Timezone is required");
                    }
                }
                continue;
            }
        }
        disable_raw_mode();
    }

    return 1;
}

static int select_disk(char *disk_name) {
    clear_screen();

    FILE *fp = popen("lsblk -d -n -o NAME,SIZE,MODEL | awk '{printf \"%s (%s) %s\\n\", $1, $2, substr($0, index($0,$3))}'", "r");
    if (fp == NULL) {
        show_message("Failed to list disks");
        return 0;
    }

    char disks[32][256];
    char names[32][64];
    int disk_count = 0;

    while (disk_count < 32 && fgets(disks[disk_count], sizeof(disks[0]), fp) != NULL) {
        disks[disk_count][strcspn(disks[disk_count], "\n")] = '\0';
        sscanf(disks[disk_count], "%s", names[disk_count]);
        disk_count++;
    }
    pclose(fp);

    if (disk_count == 0) {
        show_message("No disks found");
        return 0;
    }

    const char *disk_ptrs[32];
    for (int i = 0; i < disk_count; i++) {
        disk_ptrs[i] = disks[i];
    }

    int selected = select_from_menu(disk_ptrs, disk_count);
    if (selected < 0) {
        return 0;
    }

    strcpy(disk_name, names[selected]);

    int rows, cols;
    get_terminal_size(&rows, &cols);
    clear_screen();
    draw_logo(cols);

    int logo_start = (cols - 70) / 2;
    printf("\033[%d;%dH\033[37mWARNING: All data on \033[31m/dev/%s\033[37m will be destroyed!\033[0m", 10, logo_start, disk_name);
    printf("\033[%d;%dH\033[37mType 'yes' to confirm: \033[0m", 12, logo_start);
    fflush(stdout);

    char confirm[256];
    struct termios old_term;
    tcgetattr(STDIN_FILENO, &old_term);
    struct termios new_term = old_term;
    new_term.c_lflag |= (ECHO | ICANON);
    new_term.c_lflag &= ~ISIG;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_term);

    if (fgets(confirm, sizeof(confirm), stdin) == NULL) {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &old_term);
        return 0;
    }
    confirm[strcspn(confirm, "\n")] = '\0';
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &old_term);

    if (strcmp(confirm, "yes") != 0) {
        show_message("Installation cancelled");
        return 0;
    }

    return 1;
}

int main(void) {
    char username[256] = "";
    char password[256] = "";
    char confirmed_password[256] = "";
    char hostname[256] = "";
    char keyboard[256] = "";
    char timezone[256] = "";

    if (!get_form_input(username, password, confirmed_password, hostname, keyboard, timezone)) {
        return 1;
    }

    const char *levels[] = {
        "Beginner (We'll pick everything for you.)",
        "Intermediate (choose desktop & tools)",
        "Advanced (full customization)"
    };

    int level = select_from_menu(levels, 3);
    if (level < 0) {
        return 1;
    }

    char disk[64] = "";
    if (!select_disk(disk)) {
        return 1;
    }

    clear_screen();
    printf("Installation would proceed with:\n");
    printf("Username: %s\n", username);
    printf("Hostname: %s\n", hostname);
    printf("Keyboard: %s\n", keyboard);
    printf("Timezone: %s\n", timezone);
    printf("Level: %s\n", levels[level]);
    printf("Disk: /dev/%s\n", disk);

    return 0;
}
