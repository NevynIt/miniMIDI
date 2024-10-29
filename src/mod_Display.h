#ifndef MOD_DISPLAY_H
#define MOD_DISPLAY_H

#include "Module.h"
#include <vector>
#include <cstdint>
#include <string>

class mod_Display : public Module {
public:
    void Init() override;
    void Tick() override;

    void Clear();
    void DrawPixel(int x, int y, bool color);
    void DrawLine(int x0, int y0, int x1, int y1, bool color);
    void DrawRect(int x, int y, int w, int h, bool color);
    void FillRect(int x, int y, int w, int h, bool color);
    void DrawText(int x, int y, const std::string& text, bool color);
    void Update();

private:
    static const int WIDTH = 128;
    static const int HEIGHT = 64;
    std::vector<uint8_t> framebuffer;

    void SendCommand(uint8_t command);
    void SendData(const std::vector<uint8_t>& data);
};

#endif // MOD_DISPLAY_H