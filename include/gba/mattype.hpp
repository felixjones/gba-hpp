/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_MATTYPE_HPP
#define GBAXX_MATTYPE_HPP

#include <algorithm>
#include <cstddef>
#include <type_traits>
#include <utility>

#include <gba/tupletype.hpp>

#include <gba/util/array_traits.hpp>
#include <gba/util/tuple_traits.hpp>

namespace gba {
namespace detail {

    template <typename>
    struct is_mat_data_type : std::false_type{};

    template <typename T> requires (util::Tuple<T> || util::Array<T>) && util::is_multi_dimensional<T>
    struct is_mat_data_type<T> : std::true_type{};

    template <typename>
    struct is_uni_dimensional : std::false_type{};

    template <typename T> requires (util::Tuple<T> || util::Array<T>) && (!util::Array<std::tuple_element_t<0, T>>)
    struct is_uni_dimensional<T> : std::true_type{};

} // namespace detail

template <typename... ColumnTs> requires (util::ArraysSameSize<ColumnTs...> && std::conjunction_v<detail::is_uni_dimensional<ColumnTs>...>)
class mat {
protected:
    using data_type =  std::conditional_t<util::ArraysSameValueType<ColumnTs...>,
            std::array<std::tuple_element_t<0, tuple<ColumnTs...>>, sizeof...(ColumnTs)>,
            tuple<ColumnTs...>
        >;

    data_type m_data;
public:
    static constexpr auto rows = util::array_size<std::tuple_element_t<0, data_type>>;
    static constexpr auto columns = std::tuple_size_v<data_type>;
    static constexpr auto elements = rows * columns;

    constexpr mat() noexcept : m_data{identity()} {}

    explicit constexpr mat(ColumnTs&&... col) noexcept : m_data{std::forward<ColumnTs>(col)...} {}

    template <typename... Ts>
    explicit constexpr mat(Ts&&... elem) noexcept requires util::Array<data_type> && (sizeof...(Ts) == elements) :
        m_data{util::array_split<util::array_value_type<util::array_value_type<data_type>>, rows>(std::forward<Ts>(elem)...)} {}

    template <typename... Ts>
    explicit constexpr mat(Ts&&... elem) noexcept requires util::Tuple<data_type> && (sizeof...(Ts) == elements) :
        m_data{util::tuple_split<data_type>(std::forward<Ts>(elem)...)} {}

    template <std::size_t C>
    constexpr const auto& column() const noexcept {
        return std::get<C>(m_data);
    }

    template <std::size_t C>
    constexpr auto& column() noexcept {
        return std::get<C>(m_data);
    }

    constexpr const auto& operator[](int idx) const noexcept requires util::Array<data_type> {
        return m_data[idx];
    }

    constexpr auto& operator[](int idx) noexcept requires util::Array<data_type> {
        return m_data[idx];
    }

private:
    template <std::size_t Diag = 0u>
    static constexpr void fill_identity(data_type& result) noexcept {
        if constexpr (Diag < std::min(rows, columns)) {
            std::get<Diag>(std::get<Diag>(result)) = 1;
            fill_identity<Diag + 1u>(result);
        }
    }

    static constexpr auto identity() noexcept {
        auto result = data_type{};
        fill_identity(result);
        return result;
    }
};

} // namespace gba

#endif // define GBAXX_MATTYPE_HPP
