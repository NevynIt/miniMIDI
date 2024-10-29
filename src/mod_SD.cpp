#include "mod_SD.h"
#include <cstring>

void mod_SD::Init() {
    mounted = false;
}

void mod_SD::Tick() {
    // Perform any periodic SD card tasks if needed
}

bool mod_SD::Mount() {
    FRESULT res = f_mount(&fs, "", 1);
    mounted = (res == FR_OK);
    return mounted;
}

bool mod_SD::Unmount() {
    FRESULT res = f_mount(nullptr, "", 1);
    mounted = false;
    return (res == FR_OK);
}

bool mod_SD::WriteFile(const std::string& path, const std::string& data) {
    if (!mounted) return false;

    FIL file;
    FRESULT res = f_open(&file, path.c_str(), FA_WRITE | FA_CREATE_ALWAYS);
    if (res != FR_OK) return false;

    UINT written;
    res = f_write(&file, data.c_str(), data.size(), &written);
    f_close(&file);

    return (res == FR_OK && written == data.size());
}

bool mod_SD::ReadFile(const std::string& path, std::string& data) {
    if (!mounted) return false;

    FIL file;
    FRESULT res = f_open(&file, path.c_str(), FA_READ);
    if (res != FR_OK) return false;

    char buffer[256];
    UINT read;
    data.clear();

    while ((res = f_read(&file, buffer, sizeof(buffer), &read)) == FR_OK && read > 0) {
        data.append(buffer, read);
    }

    f_close(&file);
    return (res == FR_OK);
}