#include "mod_Lua.h"
#include "mod_Stdio.h"
#include "App.h"
#include "ast/ast_pack.h"

void mod_Lua::PreInit() {
    // Initialize Lua engine
    L = luaL_newstate();
    luaL_openlibs(L);
}

void mod_Lua::Init() {
    registerModule(this, "lua");

    // Create metatable for pack/unpack/calcsize
    luaL_Reg methods[] = {
        {"print", [](lua_State *L) -> int {
            lua_getfield(L, 1, "__field_info");
            field_info *f = (field_info *)lua_touserdata(L, -1);
            f->print();
            return 0;
        }},
        {"unpack", [](lua_State *L) -> int {
            lua_getfield(L, 1, "__field_info");
            field_info *f = (field_info *)lua_touserdata(L, -1);
            lua_pop(L, 1);
            return f->unpack(L);
        }},
        {"calcsize", [](lua_State *L) -> int {
            lua_getfield(L, 1, "__field_info");
            field_info *f = (field_info *)lua_touserdata(L, -1);
            lua_pop(L, 1);
            lua_pushinteger(L, f->calcsize());
            return 1;
        }},
        {"pack", [](lua_State *L) -> int {
            lua_getfield(L, 1, "__field_info");
            field_info *f = (field_info *)lua_touserdata(L, -1);
            lua_pop(L, 1);
            return f->pack(L);
        }},
        {nullptr, nullptr}
    };
    luaL_newmetatable(L, "PackUnpackMetaTable");
    luaL_setfuncs(L, methods, 0);
    lua_pop(L, 1); // pop the metatable

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
    auto t = lua_gettop(L);
    if (luaL_dostring(L, str) != LUA_OK) {
        LOG_WARNING("Warning: cannot execute Lua script: %s\n", lua_tostring(L, -1));
        lua_pop(L, 1);
    }
    lua_settop(L, t); //discard any return values
}

bool mod_Lua::registerModule(Module *mod, const char *name, const char *topTable)
{
    lua_getglobal(L, topTable);
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        lua_newtable(L);
        lua_setglobal(L, topTable);
        lua_getglobal(L, topTable);
    }
    lua_getfield(L, -1, name);
    if (!lua_isnil(L, -1))
    {
        LOG_WARNING("Warning: module %s already registered, mixing\n", name);
    }
    else
    {
        lua_pop(L, 1);
        lua_createtable(L, 0, 0);
        lua_setfield(L, -2, name);
        lua_getfield(L, -1, name);
    }

    for (const char **ptr = mod->GetCommands(); ptr && *ptr; ptr++) {
        lua_pushlightuserdata(L, mod);
        lua_pushinteger(L, ptr-mod->GetCommands());
        lua_pushcclosure(L, [](lua_State *L) -> int {
            Module *mod = (Module *)lua_touserdata(L, lua_upvalueindex(1));
            return mod->DoCommand(lua_tointeger(L, lua_upvalueindex(2)), L);
        }, 2);
        lua_setfield(L, -2, *ptr);
    }

    lua_pop(L, 2);
    return true;   
}

const char **mod_Lua::GetCommands() const {
    static const char *commands[] = {
        "doString",
        "dump", //dump(ptr, size) return the memory at ptr of size bytes as a string
        "compile", //compile(fmt) prepare the structures to decode according to fmt
        "ptr", //ptr(addr) return addr as lightuserdata
        nullptr
    };
    return commands;
}

int mod_Lua::DoCommand(int i, lua_State *L) {
    switch (i) {
        case 0: // doString
            doString(lua_tostring(L, 1));
            return lua_gettop(L);
        case 1: // dump
            {
                uint32_t addr;
                if (lua_islightuserdata(L, 1)) {
                    addr = (uint32_t)lua_touserdata(L, 1);
                } else {
                    addr = luaL_checkinteger(L, 1);
                } 
                uint32_t size = luaL_checkinteger(L, 2);
                uint8_t *p = (uint8_t *)addr;
                lua_pushlstring(L, (const char *)p, size);
                return 1;
            }
        case 2: // compile
            {
                const char *fmt = luaL_checkstring(L, 1);
                field_info *f = field_info::parse(fmt);
                if (!f) {
                    return luaL_error(L, "Invalid format string");
                }

                lua_newtable(L);
                luaL_getmetatable(L, "PackUnpackMetaTable"); //maybe change the name one day
                lua_setmetatable(L, -2);

                lua_pushlightuserdata(L, f);
                lua_setfield(L, -2, "__field_info");

                return 1;
            }
        case 3: //ptr
            lua_pushlightuserdata(L, (void *)luaL_checkinteger(L, 1));
            return 1;
    }
    return 0;
}
