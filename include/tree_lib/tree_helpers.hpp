/*!
 * @file tree_helpers.hpp
 * @author  (Aleksander Rybin)
 * @brief Helper classes for dispatch calls on variant
 * @date 2021-08-30
 */

#ifndef TREE_LIB_TREE_HELPERS_HPP
#define TREE_LIB_TREE_HELPERS_HPP

#pragma once

// ---------------------
// includes: STL
#include <iostream>
#include <optional>
#include <variant>

// ---------------------
// includes: local
#include "tree_lib/tree_visitors.hpp"

namespace tree_lib {

namespace helpers {

namespace detail {

template <std::size_t N>
struct VariantSwitch;

struct VariantSwitchHelper {
    template <std::size_t N>
    friend struct VariantSwitch;

   private:
    template <typename std::size_t N, typename Variant>
    void PrintByTypeIndex(std::ostream& os, const Variant& v) {
        helpers::PrintVisitor<std::variant_alternative_t<N, Variant>>{}(os, std::get<N>(v));
    }

    template <typename std::size_t N, typename Variant>
    void PackByTypeIndex(std::ostream& os, const Variant& v) {
        helpers::PackVisitor<std::variant_alternative_t<N, Variant>>{}(os, std::get<N>(v));
    }

    template <typename std::size_t N, typename Variant>
    std::optional<Variant> UnPackByTypeIndex(std::istream& is) {
        auto v = helpers::UnPackVisitor<std::variant_alternative_t<N, Variant>>{}(is);

        if (!v.has_value()) {
            return std::nullopt;
        } else {
            return {std::move(v).value()};
        }
    }
};

template <std::size_t N>
struct VariantSwitch {
    template <typename Variant>
    void Print(std::size_t index, std::ostream& os, const Variant& v) {
        if (index == N) {
            VariantSwitchHelper{}.PrintByTypeIndex<N>(os, v);
        } else {
            VariantSwitch<N - 1>{}.Print(index, os, v);
        }
    }

    template <typename Variant>
    void Pack(std::size_t index, std::ostream& os, const Variant& v) {
        if (index == N) {
            VariantSwitchHelper{}.PackByTypeIndex<N>(os, v);
        } else {
            VariantSwitch<N - 1>{}.Pack(index, os, v);
        }
    }

    template <typename Variant>
    auto UnPack(std::size_t index, std::istream& is) {
        if (index == N) {
            return VariantSwitchHelper{}.UnPackByTypeIndex<N, Variant>(is);
        } else {
            return VariantSwitch<N - 1>{}.template UnPack<Variant>(index, is);
        }
    }
};

template <>
struct VariantSwitch<0> {
    template <typename Variant>
    void Print(std::size_t index, std::ostream& os, const Variant& v) {
        VariantSwitchHelper{}.PrintByTypeIndex<0>(os, v);
    }

    template <typename Variant>
    void Pack(std::size_t index, std::ostream& os, const Variant& v) {
        VariantSwitchHelper{}.PackByTypeIndex<0>(os, v);
    }

    template <typename Variant>
    std::optional<Variant> UnPack(std::size_t index, std::istream& is) {
        if (index == 0) {
            return VariantSwitchHelper{}.UnPackByTypeIndex<0, Variant>(is);
        } else {
            return std::nullopt;
        }
    }
};

} // namespace detail

} // namespace helpers

} // namespace tree_lib

#endif // TREE_LIB_TREE_HELPERS_HPP
