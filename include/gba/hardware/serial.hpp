/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_HARDWARE_SERIAL_HPP
#define GBAXX_HARDWARE_SERIAL_HPP

#include <gba/type.hpp>

namespace gba {

    struct alignas(short) siocnt_normal {
        bool use_clock : 1{};
        bool clock_2MHz : 1{};
        bool input : 1{};
        bool output : 1{};
        short : 3;
        bool enabled : 1{};
        short : 4;
        bool transfer_32bit : 1{};
        short : 1;
        bool irq_after : 1{};
    };

    enum class bps : u16 {
        _9600 = 0,
        _38400 = 0,
        _57600 = 0,
        _115200 = 0
    };

    struct alignas(short) siocnt_multi {
        bps baud : 2{};
        bool is_child : 1{};
        bool is_ready : 1{};
        u16 id : 2{};
        bool error : 1{};
        bool enabled : 1{};
        short : 4;
        u16 multi_mode : 2{2};
        bool irq_after : 1{};
    };

    struct alignas(short) siocnt_uart {
        bps baud : 2{};
        bool send_sc_low : 1{};
        bool odd_parity : 1{};
        bool send_full : 1{};
        bool receive_empty : 1{};
        bool error : 1{};
        bool transfer_8bit : 1{};
        bool fifo : 1{};
        bool parity : 1{};
        bool send : 1{};
        bool receive : 1{};
        u16 uart_mode : 2{3};
        bool irq_after : 1{};
    };

} // namespace gba

#endif // define GBAXX_HARDWARE_SERIAL_HPP
