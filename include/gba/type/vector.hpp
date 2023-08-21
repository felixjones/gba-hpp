/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_TYPE_VECTOR_HPP
#define GBAXX_TYPE_VECTOR_HPP
/** @file */

#include <array>
#include <concepts>
#include <type_traits>

namespace gba {

    /**
     * @brief Utility for making a GNU vector with integral element type T of size N.
     *
     * @tparam T Integer type of the vector elements.
     * @tparam N Size of the vector (number of elements). Must be a power of 2 to conform to GNU vector requirements.
     *
     * @section Defining a vector of 4 integers:
     * @code{cpp}
     * #include <gba/gba.hpp>
     *
     * using ivec4 = gba::make_vector<int, 4>;
     *
     * static constexpr auto my_vector = ivec4{ 1, 2, 3, 4 };
     * @endcode
     */
    template <std::integral T, std::size_t N>
    using make_vector [[gnu::vector_size(sizeof(T) * N)]] = T;

    /**
     * @brief Concept for a GNU vector.
     *
     * @tparam T Type to test for GNU vector.
     */
    template <typename T>
    concept Vector = requires(T t, std::remove_cvref_t<decltype(t[0])> a) {
        requires std::same_as<T, make_vector<decltype(a), sizeof(t) / sizeof(a)>>;
    };

    /**
     * @brief Traits for GNU vectors.
     *
     * Provides compile-time information about a GNU vector type. Defines the underlying value type and size type, as
     * well as a compile-time constant for the size of the vector.
     *
     * @tparam T The type of the vector container.
     */
    template <Vector T>
    struct vector_traits {
        using value_type = typename std::remove_cvref_t<decltype(T{}[0])>;
        using size_type = typename std::size_t;

        static constexpr auto size = size_type(sizeof(T) / sizeof(value_type));
    };

    /**
     * @brief Converts vector x to type T.
     *
     * @tparam T Vector type to convert x to.
     * @param x Vector to be converted to T.
     * @return Vector x converted to vector type T.
     */
    template <Vector T>
    [[gnu::always_inline]]
    constexpr auto vector_cast(Vector auto&& x) noexcept {
        return __builtin_convertvector(x, T);
    }

    /**
     * @brief Static casts x to type T.
     *
     * @tparam T Type to static cast x to.
     * @param x Value to be static cast to T.
     * @return x static cast to type T.
     */
    template <typename T>
    [[gnu::always_inline]]
    constexpr auto vector_cast(auto&& x) noexcept {
        return static_cast<T>(x);
    }

    /**
     * @brief Converts a GNU vector to an array.
     *
     * The resulting array will have the same elements and order as the GNU vector.
     *
     * @param v The vector to be converted to an array.     *
     * @return The resulting array.
     *
     * @see array_to_vector()
     */
    constexpr auto vector_to_array(Vector auto v) noexcept {
        using array_type = std::array<typename vector_traits<decltype(v)>::value_type, vector_traits<decltype(v)>::size>;
        return __builtin_bit_cast(array_type, v);
    }

    /**
     * @brief Converts std::array to GNU vector.
     *
     * The resulting GNU vector will have the same elements as the original array, in the same order.
     *
     * @tparam T The type of elements in the array.
     * @tparam N The size of the array.
     * @param a The std::array to be converted.
     * @return GNU vector with the same elements as the original array.
     *
     * @sa array_to_vector()
     */
    template <typename T, std::size_t N>
    constexpr auto array_to_vector(std::array<T, N> a) noexcept {
        using vector_type = make_vector<T, N>;
        return __builtin_bit_cast(vector_type, a);
    }

    /**
     * @brief Uses RAII to temporarily copy a GNU vector to std::array.
     *
     * GNU vector extensions do not support custom types, meaning element access must always be for fundamental types.
     * vector_bind() provides a work-around, which temporarily copies the GNU vector to an std::array, and copies any
     * changes back to the GNU vector once it has left scope.
     *
     * @tparam As Type to convert the GNU vector elements to for the std::array.
     * @param v GNU vector to bind.
     *
     * @section Binding a vector of 4 integers:
     * @code{cpp}
     * #include <gba/gba.hpp>
     *
     * int main() {
     *     using namespace gba;
     *
     *     using ivec4 = make_vector<int, 4>;
     *     auto my_vector = ivec4{1, 2, 3, 4};
     *
     *     if (auto bound = vector_bind<int>(my_vector)) {
     *         std::swap(bound[0], bound[3]);
     *         std::swap(bound[1], bound[2]);
     *         // bound written back to my_vector on scope exit
     *     }
     * }
     * @endcode
     */
    template <typename As>
    constexpr auto vector_bind(Vector auto& v) noexcept {
        using vector_type = std::remove_cvref_t<decltype(v)>;

        constexpr auto vector_size = vector_traits<vector_type>::size;

        struct scoped_ref : std::array<As, vector_size> {
            constexpr explicit scoped_ref(vector_type& v) noexcept :
                std::array<As, vector_size>(__builtin_bit_cast(std::array<As, vector_size>, v)),
                m_owner{v} {}

            constexpr ~scoped_ref() noexcept {
                m_owner = __builtin_bit_cast(vector_type, *(std::array<As, vector_size>*) this);
            }

            consteval explicit operator bool() const noexcept {
                return !std::is_const_v<As>;
            }

            vector_type& m_owner;
        };

        return scoped_ref{v};
    }

} // namespace gba

#endif // define GBAXX_TYPE_VECTOR_HPP
