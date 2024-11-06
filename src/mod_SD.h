#ifndef MOD_SD_H
#define MOD_SD_H

#include "Module.h"
#include "ff.h" // Include FatFs header
#include <string>

class mod_SD : public Module {
public:
    void Init() override;
    void Tick() override;
    void Test() override;

    bool Mount();
    bool Unmount();
    bool WriteFile(const std::string& path, const std::string& data);
    bool ReadFile(const std::string& path, std::string& data);

    FATFS fs; // File system object
    bool mounted;
};

#endif // MOD_SD_H