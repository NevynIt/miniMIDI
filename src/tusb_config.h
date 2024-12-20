#ifndef _TUSB_CONFIG_H_
#define _TUSB_CONFIG_H_

#ifdef __cplusplus
 extern "C" {
#endif

// Common Configuration
#define CFG_TUSB_MCU                OPT_MCU_RP2040
#define CFG_TUSB_RHPORT0_MODE       (OPT_MODE_DEVICE | OPT_MODE_FULL_SPEED)

// Device Configuration
#include "autogen_tusb_config.h" // Autogenerated from python script

#ifdef __cplusplus
 }
#endif

#endif /* _TUSB_CONFIG_H_ */