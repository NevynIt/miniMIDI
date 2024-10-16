#ifndef MODULE_H
#define MODULE_H

class Module {
public:
    virtual ~Module() = default;
    virtual void Init() = 0;
    virtual void Tick() = 0;
};

#endif // MODULE_H