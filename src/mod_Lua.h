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
    virtual const char **GetCommands() const override;
    virtual int DoCommand(int i, lua_State *L) override;

    void doString(const char *str);

    bool registerModule(Module *mod, const char *name, const char *topTable = "mm");
    int pack(const char *fmt);
    bool pack_value(std::string &buffer, char type);
    int calcsize(const char *fmt);

    int unpack(const char *buffer, int len, const char *fmt);
    bool unpack_value(const char *buffer, int len, int &offset, char type, int array_size = 1);

    lua_State *L;
};