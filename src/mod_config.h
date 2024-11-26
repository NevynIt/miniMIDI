#ifndef MOD_CONFIG_H
#define MOD_CONFIG_H

/*
Idea: load a number of files at boot from SD and map them to memory mapped flash
do something for load levelling and to avoid copying a file if it's already mapped to flash

*/

#include "Module.h"
#include <unordered_map>
#include <string>
#include <variant>
#include <stdint.h>

template<typename T>
uint8_t getClassID() {
    static_assert(false, "Unsupported type");
    return 0;
}

struct block;

class mod_config : public Module {
public:
    void Init() override;
    void Tick() override;
    void Test() override;

    const block *find(uint8_t type, const char *name) const;
    void defrag();
    void format(int blks = 0);

    template<typename T>
    T *SetConfig(const std::string& key, const T& value);

    template<typename T>
    T *GetConfig(const std::string& key) const;

public:
    const block *cfg_start = nullptr;
    const block *cfg_end = nullptr;
    const block *cfg_free = nullptr;
};

#endif // MOD_CONFIG_H