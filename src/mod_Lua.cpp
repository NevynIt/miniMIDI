#include "mod_Lua.h"
#include "mod_Stdio.h"
#include "App.h"

void mod_Lua::PreInit() {
    // Initialize Lua engine
    L = luaL_newstate();
    luaL_openlibs(L);
}

void mod_Lua::Init() {
    registerModule(this, "lua");

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
        "unpack", //unpack(ptr, fmt) decode a struct at ptr according to fmt
        "ptr", //ptr(addr) return addr as lightuserdata
        "pack",
        "calcsize",
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
        case 2: // unpack
            {
                // Get string from Lua stack instead of address
                size_t len;
                const char *buffer = luaL_checklstring(L, 1, &len);
                const char *fmt = luaL_checkstring(L, 2);

                unpack(buffer, (int)len, fmt);
                return 1;
            }
        case 3: //ptr
            lua_pushlightuserdata(L, (void *)luaL_checkinteger(L, 1));
            return 1;
        case 4: // pack
            {
                // Ensure the data is a table and a string is provided
                luaL_checktype(L, 1, LUA_TTABLE);
                const char *fmt = luaL_checkstring(L, 2);

                return pack(fmt);
            }
        case 5: // calcsize
            {
                const char *fmt = luaL_checkstring(L, 1);
                int size = calcsize(fmt);
                if (size < 0) {
                    return luaL_error(L, "Unknown format specifier in format string");
                }
                lua_pushinteger(L, size);
                return 1;
            }
    }
    return 0;
}

/*
    for X, packing is the same as B, unpacking produces a string of hex digits
*/

#include "ast/ast_pack.h"

int mod_Lua::pack(const char *fmt)
{
    field_info *f = parse_pack_format_string(fmt);
    if (!f)
    {
        return luaL_error(L, "Invalid format string");
    }

    print_field(f);
    return 0;
}

bool mod_Lua::pack_value(std::string &buffer, char type)
{
    return false;
}
int mod_Lua::calcsize(const char *fmt)
{
    field_info *f = parse_pack_format_string(fmt);
    if (!f)
    {
        return -1;
    }

    return 0;
}

int mod_Lua::unpack(const char *buffer, int len, const char *fmt)
{
    field_info *f = parse_pack_format_string(fmt);
    if (!f)
    {
        return luaL_error(L, "Invalid format string");
    }

    return 0;
}
bool mod_Lua::unpack_value(const char *buffer, int len, int &offset, char type, int array_size)
{
    return false;
}
