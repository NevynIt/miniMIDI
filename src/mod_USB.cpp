#include "mod_USB.h"

void mod_USB::Init() {
    tusb_init();
}

void mod_USB::Tick() {
    tud_task(); // TinyUSB device task
}