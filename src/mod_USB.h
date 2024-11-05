#ifndef MOD_USB_H
#define MOD_USB_H

#include "Module.h"
#include "tusb.h"

class mod_USB : public Module {
public:
    void Init() override;
    void Tick() override;
    void Test() override;

    uint32_t getLastSOF_us() const;
    int32_t SOF_sync_us = 0; // Number of microseconds from CPU reference to USB SOF reference, always between -1ms and 0

    int count_sync_errors = 0;
};

#endif // MOD_USB_H