/*!
 * @file tree_node.hpp
 * @author  (Aleksander Rybin)
 * @brief Heterogeneous tree node class
 * @date 2021-08-29
 */

#ifndef TREE_LIB_TREE_NODE_HPP
#define TREE_LIB_TREE_NODE_HPP

#pragma once

// ---------------------
// includes: STL
#include <type_traits>
#include <variant>
#include <vector>

// ---------------------
// includes: local
#include "tree_lib/tree_helpers.hpp"

namespace tree_lib {

/*!
 * @brief Heterogeneous tree node
 *
 * @tparam Types types that node can hold
 */
template <typename... Types>
class TreeNode {
   public:
    using NodeType = TreeNode<Types...>;

   public:
    TreeNode() = default;

    template <typename T>
    TreeNode(const T& data) : m_data(data) {
        static_assert((std::is_same_v<T, Types> || ...), "Unsupported type");
    }

    template <typename T, typename std::enable_if_t<std::is_rvalue_reference_v<T>>::value>
    TreeNode(T&& data) : m_data(std::move(data)) {
        static_assert((std::is_same_v<T, Types> || ...), "Unsupported type");
    }

    bool HasChilds() const noexcept {
        return !m_childs.empty();
    }

    bool IsLeaf() const noexcept {
        return !HasChilds();
    }

    const std::vector<NodeType>& GetChilds() const noexcept {
        return m_childs;
    }

    constexpr bool HasValue() const noexcept {
        return !std::holds_alternative<std::monostate>(m_data);
    }

    constexpr std::size_t GetCurrentTypeIndex() const noexcept {
        return m_data.index();
    }

    template <typename T>
    constexpr const T& GetValueByType() const {
        static_assert((std::is_same_v<T, Types> || ...), "Unsupported type");
        return std::get<T>(m_data);
    }

    template <typename T>
    constexpr T& GetValueByType() {
        static_assert((std::is_same_v<T, Types> || ...), "Unsupported type");
        return std::get<T>(m_data);
    }

    template <std::size_t I>
    constexpr const auto& GetValueByTypeIndex() const {
        return std::get<I>(m_data);
    }

    template <std::size_t I>
    constexpr auto& GetValueByTypeIndex() {
        return std::get<I>(m_data);
    }

    NodeType& AddChild(const NodeType& child) {
        m_childs.push_back(child);
        return m_childs[m_childs.size() - 1];
    }

    NodeType& AddChild(NodeType&& child) {
        m_childs.push_back(std::move(child));
        return m_childs[m_childs.size() - 1];
    }

    void RemoveChild(const NodeType& child) {
        m_childs.erase(child);
    }

    void ClearAllChilds() {
        m_childs.clear();
    }

    void ClearValue() {
        m_data = std::monostate{};
    }

    template <typename T>
    void SetValue(const T& data) {
        static_assert((std::is_same_v<T, Types> || ...), "Unsupported type");
        m_data = data;
    }

    template <typename T>
    void SetValue(T&& data) {
        static_assert((std::is_same_v<T, Types> || ...), "Unsupported type");
        m_data = std::move(data);
    }

    void PrintNodeData(std::ostream& os) const {
        PrintNodeDataImpl(os);
    }

    void DumpNodeData(std::ostream& os) const {
        DumpNodeDataImpl(os);
    }

    bool LoadNodeData(std::istream& is) {
        return LoadNodeDataImpl(is);
    }

    bool operator==(const NodeType& other) {
        return m_data == other.m_data;
    }

   private:
    using InternalVariantStorageType = std::variant<std::monostate, Types...>;

    static constexpr auto InternalVariantStorageTypeSize = std::variant_size_v<InternalVariantStorageType>;

    static constexpr auto InternalVariantStorageTypeIndexSize = InternalVariantStorageTypeSize - 1;

    void PrintNodeDataImpl(std::ostream& os) const {
        helpers::detail::VariantSwitch<InternalVariantStorageTypeIndexSize>{}.Print(m_data.index(),
                                                                                    os,
                                                                                    m_data);
    }

    void DumpNodeDataImpl(std::ostream& os) const {
        helpers::detail::VariantSwitch<InternalVariantStorageTypeIndexSize>{}.Pack(m_data.index(),
                                                                                   os,
                                                                                   m_data);
    }

    bool LoadNodeDataImpl(std::istream& is) {
        int i;
        if (!(is >> i) || (i < 0)) {
            std::cerr << "got invalid variant type index " << i << std::endl;
            return false;
        }

        const auto typeIndex = static_cast<std::size_t>(i);
        if (typeIndex > InternalVariantStorageTypeIndexSize) {
            std::cerr << "got invalid variant type index " << typeIndex << "should be in [0, "
                      << InternalVariantStorageTypeIndexSize << "]" << std::endl;
            return false;
        }

        auto nodeData = helpers::detail::VariantSwitch<InternalVariantStorageTypeIndexSize>{}
                            .template UnPack<InternalVariantStorageType>(typeIndex, is);

        if (!nodeData.has_value()) {
            std::cerr << "can't read node data" << std::endl;
            return false;
        }

        m_data = std::move(nodeData).value();
        return true;
    }

   private:
    InternalVariantStorageType m_data;   //!< node data
    std::vector<NodeType>      m_childs; //!< child nodes from left to right
};

} // namespace tree_lib

#endif // TREE_LIB_TREE_NODE_HPP
