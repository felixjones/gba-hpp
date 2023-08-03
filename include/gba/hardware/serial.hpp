/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_HARDWARE_SERIAL_HPP
#define GBAXX_HARDWARE_SERIAL_HPP
/** @file */

#include <gba/type.hpp>

namespace gba {

    /**
     * @struct siocnt_normal
     * @brief Parameters for Serial IO in Normal mode.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000128h---siocnt---sio-control-usage-in-normal-mode-rw">4000128h - SIOCNT - SIO Control, usage in NORMAL Mode (R/W)</a>
     *
     * @sa mmio::SIOCNT_NORMAL
     */
    struct alignas(short) siocnt_normal {
        bool use_clock : 1{}; /**< Use the internal/local clock for retrieving/sending serial data. The alternative is to use the clock of a connected serial device (usually the host of a multiplayer game). */
        bool clock_2MHz : 1{}; /**< true = Fast 2MHz mode, false = Slow 256KHz mode. Recommended for accessories directly connected to the serial port. @note Link cables should set this to false. */
        bool input : 1{}; /**< Read only serial output state of the connected serial device (usually a multiplayer opponent). */
        bool output : 1{}; /**< When true the serial data will be sent when clocked. Applied when siocnt_normal::enabled is set to true. */
        short : 3;
        bool enabled : 1{}; /**< Set to true to begin the serial transfer. Will automatically reset to false when transfer is complete. */
        short : 4;
        bool transfer_32bit : 1{}; /**< Unit size of the serial transfer. true = 32-bit, false = 8-bit. @sa mmio::SIODATA8 @sa mmio::SIODATA32 */
        short : 1;
        bool irq_after : 1{}; /**< Trigger an interrupt when transfer is complete. @sa irq::serial */
    };

    /**
     * @enum bps
     * @brief Baud rate bits-per-second for Serial IO in Multi-Player mode or UART mode.
     *
     * @sa siocnt_multi
     * @sa siocnt_uart
     */
    enum class bps : u16 {
        _9600 = 0,
        _38400 = 1,
        _57600 = 2,
        _115200 = 3
    };

    /**
     * @struct siocnt_multi
     * @brief Parameters for Serial IO in Multi-Player mode.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000128h---siocnt---sio-control-usage-in-multi-player-mode-rw">4000128h - SIOCNT - SIO Control, usage in MULTI-PLAYER Mode (R/W)</a>
     *
     * @sa mmio::SIOCNT_MULTI
     */
    struct alignas(short) siocnt_multi {
        bps baud : 2{}; /**< @sa bps */
        bool is_child : 1{}; /**< Read only flag if this device is a multiplayer child. false = parent/host. */
        bool is_ready : 1{}; /**< Read only connection status of the multiplayer devices. True = devices are ready. */
        u16 id : 2{}; /**< Read only multiplayer ID number. @note Parents are always ID 0. @sa siocnt_multi::is_child */
        bool error : 1{}; /**< Read only error state of the multiplayer connection. */
        bool enabled : 1{}; /**< Start flag to trigger multiplayer data transfer. @note This is read only for child devices and acts as a "busy" flag. */
        short : 4;
        u16 multi_mode : 2{2}; /**< @warning Must be set to 2. */
        bool irq_after : 1{}; /**< Trigger an interrupt when transfer is complete. @sa irq::serial */
    };

    /**
     * @struct siocnt_uart
     * @brief Parameters for Serial IO in UART mode.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000128h---sccnt_l---sio-control-usage-in-uart-mode-rw">4000128h - SCCNT_L - SIO Control, usage in UART Mode (R/W)</a>
     *
     * @sa mmio::SIOCNT_UART
     */
    struct alignas(short) siocnt_uart {
        bps baud : 2{}; /**< @sa bps */
        bool send_sc_low : 1{}; /**< When true data will only be sent when SC is low. */
        bool odd_parity : 1{}; /**< Parity control: false = even, true = odd. */
        bool send_full : 1{}; /**< Read only flag when the send buffer is full/ready. */
        bool receive_empty : 1{}; /**< Read only flag when the receive buffer is empty/ready. */
        bool error : 1{}; /**< Read only error state of the UART connection. */
        bool transfer_8bit : 1{}; /**< Unit size of the serial transfer. true = 8-bit, false = 7-bit. @sa mmio::SIODATA8 */
        bool fifo : 1{}; /**< Enable first-in-first-out mode. */
        bool parity : 1{}; /**< Enable parity mode. */
        bool send : 1{}; /**< Enable sending. */
        bool receive : 1{}; /**< Enable receiving. */
        u16 uart_mode : 2{3}; /**< @warning Must be set to 3. */
        bool irq_after : 1{}; /**< Trigger an interrupt when transfer is complete. @sa irq::serial */
    };

    /**
     * @struct rcnt_joybus
     * @brief Used to select JOY BUS serial mode with rcnt_joybus::joybus_mode.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000134h---rcnt-r---mode-selection-in-joy-bus-mode-rw">4000134h - RCNT (R) - Mode Selection, in JOY BUS mode (R/W)</a>
     *
     * @note JOY BUS, or Joybus, is a proprietary serial protocol for communicating with accessories.
     *
     * @sa mmio::RCNT_JOYBUS
     */
    struct alignas(short) rcnt_joybus {
        bool clock : 1{}; /**< Current SC state. @sa rcnt_gpio::data */
        bool direction : 1{}; /**< Current SD state. @sa rcnt_gpio::data */
        bool input : 1{}; /**< Current SI state. @sa rcnt_gpio::data */
        bool output : 1{}; /**< Current SO state. @sa rcnt_gpio::data */
        short : 10;
        u16 joybus_mode : 2{3}; /**< @warning Must be set to 3. */
    };

    /**
     * @struct joycnt
     * @brief Parameters for Serial IO in JOY BUS mode.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000140h---joycnt---joy-bus-control-register-rw">4000140h - JOYCNT - JOY BUS Control Register (R/W)</a>
     *
     * @note JOY BUS, or Joybus, is a proprietary serial protocol for communicating with accessories.
     *
     * @sa mmio::JOYCNT
     */
    struct alignas(int) joycnt {
        bool reset : 1{}; /**< True when device has reset. Write to acknowledge. @sa joycnt::irq_on_reset */
        bool receive : 1{}; /**< True when receiving is complete. Write to acknowledge. */
        bool send : 1{}; /**< True when sending is complete. Write to acknowledge. */
        short : 3;
        bool irq_on_reset : 1{}; /**< Trigger an interrupt when receiving a device reset command. @sa irq::serial */
    };

    /**
     * @struct joystat
     * @brief Status structure for Serial IO in Joy-Bus mode.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000158h---joystat---receive-status-register-rw">4000158h - JOYSTAT - Receive Status Register (R/W)</a>
     *
     * @note Joy bus, or Joybus, is a proprietary serial protocol for communicating with accessories.
     *
     * @sa mmio::JOYSTAT
     */
    struct alignas(int) joystat {
        int : 1;
        bool receive : 1{}; /**< Set to true when writing to mmio::JOY_TRANS. @sa mmio::JOY_TRANS */
        int : 1;
        bool send : 1{}; /**< Set to false when reading from mmio::JOY_RECV. @sa mmio::JOY_RECV */
        u32 general_purpose : 2{};
    };

    /**
     * @struct rcnt_gpio
     * @brief Parameters for Serial IO in General-Purpose mode.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000134h---rcnt-r---sio-mode-usage-in-general-purpose-mode-rw">4000134h - RCNT (R) - SIO Mode, usage in GENERAL-PURPOSE Mode (R/W)</a>
     *
     * @sa mmio::RCNT_GPIO
     */
    struct alignas(short) rcnt_gpio {
        u16 data : 4{}; /**< 4-bit data. Direction mask controls if this is read/write. @sa rcnt_gpio::direction */
        u16 direction : 4{}; /**< 4-bit data direction mask. 0 = read, 1 = write. @sa rcnt_gpio::data */
        bool irq_after : 1{}; /**< Trigger an interrupt when transfer is complete. @sa irq::serial */
        short : 5;
        u16 gpio_mode : 2{2}; /**< @warning Must be set to 2. */
    };

    /**
     * @struct multi_boot_param
     * @brief Parameters for MultiBoot.
     * @see <a href="https://mgba-emu.github.io/gbatek/#multiboot-parameter-structure">Multiboot Parameter Structure</a>
     *
     * @sa bios::MultiBoot()
     */
    struct multi_boot_param {
        u32	reserved1[5];
        u8 handshake_data;
        u8 padding;
        u16	handshake_timeout;
        u8 probe_count;
        u8 client_data[3];
        u8 palette_data;
        u8 response_bit;
        u8 client_bit;
        u8 reserved2;
        u8* boot_srcp;
        u8* boot_endp;
        u8* masterp;
        u8* reserved3[3];
        u32	system_work2[4];
        u8 sendflag;
        u8 probe_target_bit;
        u8 check_wait;
        u8 server_type;
    };

    /**
     * @enum multi_boot_mode
     * @brief Transfer mode & speed for MultiBoot.
     * @see <a href="https://mgba-emu.github.io/gbatek/#swi-25h-gba---multiboot">SWI 25h (GBA) - MultiBoot</a>
     *
     * @sa bios::MultiBoot()
     */
    enum class multi_boot_mode : int {
        normal_256KHz_32bit = 0,
        multi_play_115KHz_16bit = 1,
        normal_2MHz_32bit = 2,
    };

} // namespace gba

#endif // define GBAXX_HARDWARE_SERIAL_HPP
