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

class mod_config : public Module {
public:
    void Init() override;
    void Tick() override;

    template<typename T>
    void SetConfig(const std::string& key, const T& value);

    template<typename T>
    T GetConfig(const std::string& key) const;

private:
    using ConfigValue = std::variant<int, float, std::string>;
    std::unordered_map<std::string, ConfigValue> configMap;
};

#endif // MOD_CONFIG_H