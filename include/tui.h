#ifndef TUI_H
#define TUI_H

#include "types.h"

void tui_init(void);
void tui_cleanup(void);
bool tui_run(install_config *config);

#endif
