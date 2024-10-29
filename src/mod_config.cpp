#include "mod_config.h"
#include <stdexcept>

void mod_config::Init() {
    // Initialize configuration settings if needed
}

void mod_config::Tick() {
    // Perform any periodic configuration tasks if needed
}

template<typename T>
void mod_config::SetConfig(const std::string& key, const T& value) {
    configMap[key] = value;
}

template<typename T>
T mod_config::GetConfig(const std::string& key) const {
    auto it = configMap.find(key);
    if (it != configMap.end()) {
        return std::get<T>(it->second);
    }
    throw std::runtime_error("Configuration key not found");
}