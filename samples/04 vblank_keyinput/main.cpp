#include <gba/gba.hpp>

int main() {
    using namespace gba;

    mmio::IRQ_HANDLER = agbabi::irq_empty; // Empty IRQ handler provided by agbabi
    mmio::DISPSTAT = {.irq_vblank = true};
    mmio::IE = {.vblank = true};

    mmio::IME = true;

    keystate keys{};
    while (true) {
        if (keys = *mmio::KEYINPUT; keys.held(key::reset_combo)) {
            break;
        }

        bios::VBlankIntrWait();
    }

    mmio::DISPCNT = {.forced_blank = true};
    mmio::IME = false;
    do {
        keys = *mmio::KEYINPUT;
    } while (keys.held(key::reset_combo));
}
