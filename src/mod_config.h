#ifndef MOD_CONFIG_H
#define MOD_CONFIG_H

#include "Module.h"
#include <string>
#include <stdint.h>

template<typename T>
uint8_t getClassID() {
    return 0; //default type, no pointers
}

struct block;

class mod_config : public Module {
public:
    void Init() override;
    void Tick() override;
    void Test() override;

    void load(); //load the configuration from the sd card and update the flash memory

    const block *find(uint8_t type, const char *name) const;
    void defrag();
    void format(int blks = 0);

    template<typename T>
    T *SetConfig(const std::string& key, const T& value)
    {
        return (T *)SetConfig_impl(key, getClassID<T>(), &value, sizeof(T));
    }

    template<typename T>
    T *GetConfig(const std::string& key) const
    {
        return (T *)GetConfig_impl(key, getClassID<T>());
    }

    const void *GetConfig_impl(const std::string& key, uint8_t id) const;
    const void *SetConfig_impl(const std::string& key, uint8_t id, const void *value, uint16_t size);

public:
    const block *cfg_start = nullptr;
    const block *cfg_end = nullptr;
    const block *cfg_free = nullptr;
    bool valid = true;
};

#endif // MOD_CONFIG_H