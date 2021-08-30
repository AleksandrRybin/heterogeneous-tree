/*!
 * @file tree_visitors.hpp
 * @author  (Aleksander Rybin)
 * @brief Classes to handle printing, saving and loading tree node data
 * @date 2021-08-29
 */

#ifndef TREE_LIB_TREE_VISITORS_HPP
#define TREE_LIB_TREE_VISITORS_HPP

#pragma once

// ---------------------
// includes: STL
#include <iostream>
#include <optional>
#include <string>
#include <variant>

namespace tree_lib {

namespace helpers {

template <typename T>
struct PrintVisitor {
    void operator()(std::ostream& os, const T& v) {
        static_assert(std::is_arithmetic_v<T>, "No template specialization for non intergral type provided");
        os << v;
    }
};

template <>
void PrintVisitor<std::string>::operator()(std::ostream& os, const std::string& v) {
    os << '"' << v << '"';
}

template <>
void PrintVisitor<std::monostate>::operator()(std::ostream& os, const std::monostate& v) {
    os << "<->";
}

template <typename T>
struct PackVisitor {
    void operator()(std::ostream& os, const T& v) {
        static_assert(std::is_arithmetic_v<T>, "No template specialization for non intergral type provided");
        os << v;
    }
};

template <>
void PackVisitor<std::string>::operator()(std::ostream& os, const std::string& v) {
    os << v;
}

template <>
void PackVisitor<std::monostate>::operator()(std::ostream& os, const std::monostate& v) {
}

template <typename T>
struct UnPackVisitor {
    std::optional<T> operator()(std::istream& is) {
        static_assert(std::is_arithmetic_v<T>, "No template specialization for non intergral type provided");

        T v;

        if (!(is >> v)) {
            std::cerr << "can't read node data got " << v << std::endl;
            return std::nullopt;
        }

        return {std::move(v)};
    }
};

template <>
std::optional<std::string> UnPackVisitor<std::string>::operator()(std::istream& is) {
    std::string v;
    if (!(is >> v)) {
        std::cerr << "can't read node data got " << v << std::endl;
        return std::nullopt;
    }

    return {std::move(v)};
}

template <>
std::optional<std::monostate> UnPackVisitor<std::monostate>::operator()(std::istream& is) {
    return std::monostate{};
}

} // namespace helpers

} // namespace tree_lib

#endif // TREE_LIB_TREE_VISITORS_HPP
