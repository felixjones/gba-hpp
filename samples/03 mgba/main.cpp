#include <gba/gba.hpp>

int main() {
    using namespace gba;

    if (mgba::open()) {
        mgba::puts(mgba::log::info, "Hello, World!");
        mgba::close();
    }

    while (true);
}
