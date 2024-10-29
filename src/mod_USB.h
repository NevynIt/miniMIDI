#ifndef MOD_USB_H
#define MOD_USB_H

#include "Module.h"
#include "tusb.h"

class mod_USB : public Module {
public:
    void Init() override;
    void Tick() override;
};

#endif // MOD_USB_H