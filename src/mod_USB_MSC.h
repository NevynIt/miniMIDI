#pragma once

#include "module.h"

class mod_USB_MSC : public Module
{
public:
    void Init() override;
    void Tick() override;
    void Test() override;

};