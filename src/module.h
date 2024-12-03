#ifndef MODULE_H
#define MODULE_H
#include "lua.hpp"
//typedef int (*lua_CFunction) (lua_State *L);

class Module {
public:
    virtual ~Module() = default;
    virtual void Init() { };
    virtual void Tick() { };
    virtual void Test() { };
    virtual const char **GetCommands() const { return nullptr; };
    virtual int DoCommand(int i, lua_State *L) { return 0; };
};

#endif // MODULE_H