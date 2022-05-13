/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_UTIL_LZ77_COMPRESS_HPP
#define GBAXX_UTIL_LZ77_COMPRESS_HPP

#include <algorithm>
#include <array>
#include <cstddef>
#include <variant>

#include <gba/inttype.hpp>

#include <gba/bios/decompress.hpp>

#include <gba/util/byte_array.hpp>
#include <gba/util/constexpr.hpp>

namespace gba::util {
namespace detail::lz77 {

struct jump_type {
    constexpr jump_type() noexcept : jump{}, length{} {}
    constexpr jump_type(std::size_t jmp, std::size_t len) noexcept : jump(jmp), length(len) {}

    [[nodiscard]]
    constexpr auto to_bytes() const noexcept {
        std::array<std::byte, 2> bytes {};

        const auto jmp = jump - 1;
        const auto msb = (jmp & 0xf00) >> 8;
        const auto lsb = jmp & 0xff;
        const auto len = (length - 3) & 0x0f;

        bytes[0] = std::byte(msb | (len << 4));
        bytes[1] = std::byte(lsb);
        return bytes;
    }

    std::size_t jump;
    std::size_t length;
};

struct block_type {
    enum class type : bool {
        byte = false,
        jump = true
    };

    [[nodiscard]]
    constexpr auto is_byte() const noexcept {
        return std::holds_alternative<std::byte>(value);
    }

    [[nodiscard]]
    constexpr auto is_jump() const noexcept {
        return std::holds_alternative<jump_type>(value);
    }

    [[nodiscard]]
    constexpr auto get_byte() const noexcept {
        return std::get<std::byte>(value);
    }

    [[nodiscard]]
    constexpr auto get_jump() const noexcept {
        const auto& jump = std::get<jump_type>(value);
        return jump.to_bytes();
    }

    std::variant<std::byte, jump_type> value;
};

class block_buffer {
public:
    constexpr void push(const block_type& block) noexcept {
        m_buffer[m_size++] = block;
    }

    [[nodiscard]]
    constexpr auto size() const noexcept {
        return m_size;
    }

    constexpr auto flush() noexcept {
        struct span_type {
            std::array<std::byte, 17> data{};
            std::size_t size{};
        };

        span_type buffer{};
        buffer.size = 1;

        uint8 bitMask = 0;
        for (std::size_t ii = 0; ii < m_size; ++ii) {
            const auto& block = m_buffer[ii];

            if (block.is_jump()) {
                bitMask |= 1 << (7 - ii);

                const auto jump = block.get_jump();
                buffer.data[buffer.size++] = jump[0];
                buffer.data[buffer.size++] = jump[1];
            } else {
                buffer.data[buffer.size++] = block.get_byte();
            }
        }

        buffer.data[0] = std::byte(bitMask);

        m_size = 0;
        return buffer;
    }
private:
    std::array<block_type, 8> m_buffer{};
    std::size_t m_size{};
};

class memory_buffer {
public:
    constexpr void push(std::byte value) noexcept {
        if (m_size == m_buffer.size()) {
            for (std::size_t ii = 1; ii < m_size; ++ii) {
                m_buffer[ii - 1] = m_buffer[ii];
            }
            --m_size;
        }
        m_buffer[m_size++] = value;
    }

    [[nodiscard]]
    constexpr auto operator[](std::size_t idx) const noexcept {
        return m_buffer[idx];
    }

    [[nodiscard]]
    constexpr auto size() const noexcept {
        return m_size;
    }

    [[nodiscard]]
    constexpr auto next_index(std::byte value, std::size_t start) const noexcept {
        for (std::size_t ii = start; ii < m_size; ++ii) {
            if (m_buffer[ii] == value) {
                return ii;
            }
        }
        return std::size_t(-1);
    }
private:
    std::array<std::byte, 0x1000> m_buffer;
    std::size_t m_size;
};

consteval auto compress_32mib(const ByteArray auto& data) noexcept {
    struct compressed_type {
        std::array<std::byte, 0x2000000> data;
        std::size_t uncompressed_length;
        std::size_t compressed_length;
    };

    constexpr auto size = array_size<decltype(data)>;

    compressed_type compressed{};
    compressed.uncompressed_length = array_size<decltype(data)>;
    compressed.compressed_length = 0;

    auto& result = compressed.data;
    auto& resultLength = compressed.compressed_length;

    auto blockBuffer = block_buffer{};
    auto memoryBuffer = memory_buffer{};

    for (std::size_t ii = 0; ii < size; ++ii) {
        const auto byte = data[ii];

        jump_type bestJump{};
        if (memoryBuffer.size() >= 3) {
            std::size_t jj = memoryBuffer.next_index(byte, 0);
            while (jj != std::size_t(-1)) {
                std::size_t kk, length{};
                for (kk = jj + 1; kk < memoryBuffer.size(); ++kk) {
                    length = kk - jj;
                    if (length == 17 || data[ii + length] != memoryBuffer[kk]) {
                        break;
                    }
                }

                if (length > bestJump.length) {
                    const auto backJump = memoryBuffer.size() - jj;
                    bestJump = jump_type{backJump, length};
                }

                jj = memoryBuffer.next_index(byte, jj + 1);
            }
        }

        if (bestJump.length >= 3) {
            blockBuffer.push(block_type{bestJump});
            for (std::size_t jj = 0; jj < bestJump.length; ++jj) {
                memoryBuffer.push(data[ii++]);
            }
            --ii;
        } else {
            blockBuffer.push(block_type{byte});
            memoryBuffer.push(byte);
        }

        if (blockBuffer.size() == 8) {
            const auto span = blockBuffer.flush();
            for (std::size_t jj = 0; jj < span.size; ++jj) {
                result[resultLength++] = span.data[jj];
            }
        }
    }

    if (blockBuffer.size()) {
        const auto spare = 8 - blockBuffer.size();
        const auto span = blockBuffer.flush();
        for (std::size_t ii = 0; ii < span.size; ++ii) {
            result[resultLength++] = span.data[ii];
        }

        // Pad the spare bytes
        resultLength += spare;
    }

    // Pad to multiple of 4
    resultLength = ((resultLength + 3) / 4) * 4;

    return compressed;
}

consteval auto compress(auto callable) noexcept {
    constexpr auto data_32mib = compress_32mib(callable());

    struct lz77_type : bios::uncomp_header {
        std::array<std::byte, data_32mib.compressed_length> data;
    };

    lz77_type compressed{};
    compressed.type = bios::uncomp_header::type::lz77;
    compressed.length = data_32mib.uncompressed_length;

    std::copy(data_32mib.data.cbegin(), std::next(data_32mib.data.cbegin(), data_32mib.compressed_length), compressed.data.begin());

    return compressed;
}

} // namespace detail::lz77

consteval auto& lz77_compress(auto callable) noexcept {
    return make_static<detail::lz77::compress(callable)>;
}

} // namespace gba::util

#endif // define GBAXX_UTIL_LZ77_COMPRESS_HPP
