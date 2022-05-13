/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_UTIL_HUFF_COMPRESS_HPP
#define GBAXX_UTIL_HUFF_COMPRESS_HPP

#include <algorithm>
#include <array>
#include <cstddef>
#include <iterator>
#include <type_traits>

#include <gba/inttype.hpp>

#include <gba/bios/decompress.hpp>

#include <gba/util/byte_array.hpp>
#include <gba/util/constexpr.hpp>

namespace gba::util {
namespace detail::huff {

template <typename T, std::size_t N, class Compare>
class constexpr_priority_queue {
private:
public:
    constexpr explicit constexpr_priority_queue(const Compare& compare) noexcept : m_compare{compare} {}

    constexpr void push(const T& value) noexcept {
        m_buffer[m_size++] = value;
        std::sort(m_buffer.begin(), std::next(m_buffer.begin(), m_size), m_compare);
    }

    [[nodiscard]]
    constexpr const T& top() const noexcept {
        return m_buffer[0];
    }

    [[nodiscard]]
    constexpr auto size() const noexcept {
        return m_size;
    }

    constexpr void pop() noexcept {
        if (!m_size) {
            return;
        }
        for (std::size_t ii = 1; ii < m_size; ++ii) {
            m_buffer[ii - 1] = m_buffer[ii];
        }
        --m_size;
    }

private:
    const Compare& m_compare;
    std::array<T, N> m_buffer{};
    std::size_t m_size{};
};

struct node_type {
    std::byte data{};
    std::size_t weight{};
    node_type* node0{};
    node_type* node1{};

    [[nodiscard]]
    consteval auto is_leaf() const noexcept {
        return node0 == nullptr && node1 == nullptr;
    }

    [[nodiscard]]
    constexpr auto is_branch() const noexcept {
        return node0 != nullptr && node1 != nullptr;
    }
};

template <std::size_t N>
class node_allocator {
public:
    [[nodiscard]]
    constexpr auto* alloc(std::byte data, std::size_t weight) noexcept {
        auto& node = m_buffer[m_size++];
        node.data = data;
        node.weight = weight;
        node.node0 = node.node1 = nullptr;
        return &node;
    }

    [[nodiscard]]
    constexpr auto* alloc(std::size_t weight, node_type* node0, node_type* node1) noexcept {
        auto& node = m_buffer[m_size++];
        node.data = {};
        node.weight = weight;
        node.node0 = node0;
        node.node1 = node1;
        return &node;
    }

private:
    std::array<node_type, N> m_buffer{};
    std::size_t m_size{};
};

[[nodiscard]]
constexpr bool node_compare(const node_type* node0, const node_type* node1) noexcept {
    return node0->weight < node1->weight;
}

struct bit_code {
    constexpr auto operator+(int i) const noexcept {
        auto copy = bit_code{value, length};
        if (i) {
            copy.value |= (1ull << length);
        }
        ++copy.length;
        return copy;
    }

    uint64 value{};
    std::size_t length{};
};

consteval void encode(const node_type* node, bit_code code, std::array<bit_code, 0x100>& bitCodes) noexcept {
    if (node->is_branch()) {
        encode(node->node0, code + 0, bitCodes);
        encode(node->node1, code + 1, bitCodes);
    } else {
        bitCodes[static_cast<int>(node->data)] = code;
    }
}

template <std::size_t N>
class flat_node_tree {
public:
    constexpr explicit flat_node_tree(const node_type* node) noexcept {
        m_buffer[m_size++] = node;
        flatten(node);
    }

    [[nodiscard]]
    constexpr const auto* operator[](std::size_t idx) const noexcept {
        return m_buffer[idx];
    }

    [[nodiscard]]
    constexpr auto size() const noexcept {
        return m_size;
    }

    [[nodiscard]]
    constexpr auto index_of(const node_type* node) const noexcept {
        for (std::size_t ii = 0; ii < m_size; ++ii) {
            if (m_buffer[ii] == node) {
                return ii;
            }
        }
        return std::size_t(-1);
    }

    [[nodiscard]]
    constexpr auto index_of(const node_type& node) const noexcept {
        return index_of(&node);
    }

private:
    constexpr void flatten(const node_type* node) noexcept {
        if (node->is_branch()) {
            m_buffer[m_size++] = node->node0;
            m_buffer[m_size++] = node->node1;

            flatten(node->node0);
            flatten(node->node1);
        }
    }

    std::array<const node_type*, N> m_buffer{};
    std::size_t m_size{};
};

template <std::size_t BitLength>
consteval auto compress_32mib(const ByteArray auto& data) noexcept {
    struct compressed_type {
        std::array<std::byte, 0x2000000> data;
        std::size_t uncompressed_length;
        std::size_t compressed_length;
    };

    compressed_type compressed{};
    compressed.uncompressed_length = array_size<decltype(data)>;
    compressed.compressed_length = 0;

    std::array<std::size_t, 0x100> frequencies{};
    for (std::size_t ii = 0; ii < array_size<decltype(data)>; ++ii) {
        const auto byte = static_cast<int>(data[ii]);
        if constexpr (BitLength == 4) {
            ++frequencies[byte & 0xf];
            ++frequencies[(byte >> 4) & 0xf];
        } else {
            ++frequencies[byte];
        }
    }

    node_allocator<0x400> nodeAllocator{};

    constexpr_priority_queue<node_type*, 0x400, decltype(node_compare)> pq(node_compare);
    for (std::size_t ii = 0; ii < frequencies.size(); ++ii) {
        if (frequencies[ii]) {
            pq.push(nodeAllocator.alloc(std::byte(ii), frequencies[ii]));
        }
    }

    if (pq.size() <= 1) {
        // Constant value, make root point to it twice
        auto* node01 = pq.top();
        pq.pop();
        pq.push(nodeAllocator.alloc(node01->weight + node01->weight, node01, node01));
    } else {
        while (pq.size() > 1) {
            auto* node0 = pq.top();
            pq.pop();
            auto* node1 = pq.top();
            pq.pop();

            pq.push(nodeAllocator.alloc(node0->weight + node1->weight, node0, node1));
        }
    }

    auto* root = pq.top();

    std::array<bit_code, 0x100> bitcodes{};
    encode(root, bit_code(), bitcodes);

    std::array<std::byte, 0x400> treeTable{};
    std::size_t treeTableLen = 0;

    flat_node_tree<0x400> nodes(root);
    for (std::size_t ii = 0; ii < nodes.size(); ++ii) {
        const auto& n = *nodes[ii];

        if (n.is_branch()) {
            const auto offset = (nodes.index_of(n.node0) - nodes.index_of(n)) - 1;
            auto jump = offset / 2;
            if (n.node0->is_leaf()) {
                jump |= 0x80;
            }
            if (n.node1->is_leaf()) {
                jump |= 0x40;
            }
            treeTable[treeTableLen++] = std::byte(jump);
        } else {
            treeTable[treeTableLen++] = n.data;
        }
    }

    auto& result = compressed.data;
    auto& resultLength = compressed.compressed_length;

    // Remember where we will write tableSize
    auto dataOffsetCur = resultLength++;

    // Write treeTable
    for (std::size_t ii = 0; ii < treeTableLen; ++ii) {
        result[resultLength++] = treeTable[ii];
    }

    std::array<bit_code, 0x2000000> bitArray{};
    std::size_t bitArrayLen = 0;

    for (const auto& b: data) {
        const auto byte = static_cast<int>(b);
        if constexpr (BitLength == 4) {
            bitArray[bitArrayLen++] = bitcodes[byte & 0xf];
            bitArray[bitArrayLen++] = bitcodes[(byte >> 4) & 0xf];
        } else {
            bitArray[bitArrayLen++] = bitcodes[byte];
        }
    }

    resultLength = (((resultLength + 3) / 4)) * 4;

    result[dataOffsetCur] = std::byte((resultLength / 2) - 1);

    uint32 word = 0;
    std::size_t wordLen = 32;
    for (std::size_t ii = 0; ii < bitArrayLen; ++ii) {
        const auto& bc = bitArray[ii];

        for (std::size_t jj = 0; jj < bc.length; ++jj) {
            --wordLen;
            if ((bc.value >> jj) & 1) {
                word |= (1ull << wordLen);
            }

            if (wordLen == 0) {
                result[resultLength++] = std::byte(word >> 0);
                result[resultLength++] = std::byte(word >> 8);
                result[resultLength++] = std::byte(word >> 16);
                result[resultLength++] = std::byte(word >> 24);
                word = 0;
                wordLen = 32;
            }
        }
    }

    if (wordLen < 32) {
        result[resultLength++] = std::byte(word >> 0);
        result[resultLength++] = std::byte(word >> 8);
        result[resultLength++] = std::byte(word >> 16);
        result[resultLength++] = std::byte(word >> 24);
    }

    return compressed;
}

consteval auto compress_8(auto callable) noexcept {
    constexpr auto& data_32mib = make_static<compress_32mib<8>(callable())>;

    struct huffman_type : bios::uncomp_header {
        std::array<std::byte, data_32mib.compressed_length> data;
    };

    huffman_type compressed{};
    compressed.type = bios::uncomp_header::type::huffman_8;
    compressed.length = ((data_32mib.uncompressed_length + 3) >> 2) << 2;

    std::copy(data_32mib.data.cbegin(), std::next(data_32mib.data.cbegin(), data_32mib.compressed_length), compressed.data.begin());

    return compressed;
}

consteval auto compress_4(auto callable) noexcept {
    constexpr auto& data_32mib = make_static<compress_32mib<4>(callable())>;

    struct huffman_type : bios::uncomp_header {
        std::array<std::byte, data_32mib.compressed_length> data;
    };

    huffman_type compressed{};
    compressed.type = bios::uncomp_header::type::huffman_4;
    compressed.length = ((data_32mib.uncompressed_length + 3) >> 2) << 2;

    std::copy(data_32mib.data.cbegin(), std::next(data_32mib.data.cbegin(), data_32mib.compressed_length), compressed.data.begin());

    return compressed;
}

} // namespace detail::huff

consteval auto& huffman_compress_8(auto callable) noexcept {
    return make_static<detail::huff::compress_8(callable)>;
}

consteval auto& huffman_compress_4(auto callable) noexcept {
    return make_static<detail::huff::compress_4(callable)>;
}

} // namespace gba::util

#endif // define GBAXX_UTIL_HUFF_COMPRESS_HPP
