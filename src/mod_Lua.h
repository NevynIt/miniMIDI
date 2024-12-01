#pragma once

#include "Module.h"
#include "hwConfig.h"
#include "mod_Stdio.h"
#include "lua.hpp"

class mod_Lua : public Module {
public:
    void PreInit();
    void Init() override;
    void Tick() override;

    void doString(const char *str);

    lua_State *L;
};