#include "mod_Lua.h"
#include "mod_Stdio.h"
#include "App.h"

void mod_Lua::PreInit() {
    // Initialize Lua engine
    L = luaL_newstate();
    luaL_openlibs(L);
}

void mod_Lua::Init() {
    // Load Lua boot script, which should configure the system
    if (luaL_dofile(L, "boot.lua") != LUA_OK) {
        // LOG_WARNING("Warning: cannot load Lua script: %s\n", lua_tostring(L, -1));
        lua_pop(L, 1);
    }

    printf("Lua initialized\n");
}

void mod_Lua::Tick() {
    // Do nothing
}

void mod_Lua::doString(const char *str) {
    if (luaL_dostring(L, str) != LUA_OK) {
        LOG_WARNING("Warning: cannot execute Lua script: %s\n", lua_tostring(L, -1));
        lua_pop(L, 1);
    }
}