#ifndef MODULE_H
#define MODULE_H

class Module {
public:
    virtual ~Module() = default;
    virtual void Init() { };
    virtual void Tick() { };
    virtual void Test() { };
};

#endif // MODULE_H