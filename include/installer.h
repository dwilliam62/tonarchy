#ifndef INSTALLER_H
#define INSTALLER_H

#include "types.h"

bool install_system(const install_config *config);
bool install_packages(const install_config *config);
bool configure_system(const install_config *config);
bool setup_bootloader(void);

#endif
