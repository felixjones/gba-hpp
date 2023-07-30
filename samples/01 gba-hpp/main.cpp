#include <gba/gba.hpp>

int main() {
    using namespace gba;

    mmio::DISPCNT = {.video_mode = 3, .show_bg2 = true};

    mmio::VIDEO3_VRAM[80][120] = 0x001f;
    mmio::VIDEO3_VRAM[80][136] = 0x03e0;
    mmio::VIDEO3_VRAM[96][120] = 0x7c00;

    while (true);
}
