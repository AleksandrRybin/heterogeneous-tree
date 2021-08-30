/*!
 * @file tree.hpp
 * @author  (Aleksander Rybin)
 * @brief Heterogeneous tree class
 * @date 2021-08-29
 */

#ifndef TREE_LIB_TREE_HPP
#define TREE_LIB_TREE_HPP

#pragma once

// ---------------------
// includes: STL
#include <sstream>
#include <stack>

// ---------------------
// includes: local
#include "tree_lib/tree_node.hpp"

namespace tree_lib {

/*!
 * @brief Class for working with heterogeneous trees
 *
 * @tparam Types types that tree nodes can hold
 */
template <typename... Types>
class Tree {
   public:
    using NodeType = typename TreeNode<Types...>::NodeType;

   public:
    Tree() = default;

    Tree(const NodeType& root) : m_root(root) {
    }

    Tree(NodeType&& root) : m_root(std::move(root)) {
    }

    const NodeType& GetRoot() const noexcept {
        return m_root;
    }

    NodeType& GetRoot() noexcept {
        return m_root;
    }

    bool Load(std::istream& is) {
        return LoadImpl(is);
    }

    std::string Print() const {
        std::ostringstream oss;
        PrintImpl(oss, "", m_root, false);
        return oss.str();
    }

    void Print(std::ostream& os) const {
        os << Print();
    }

    std::string Dump() const {
        std::ostringstream oss;
        DumpImpl(oss);
        return oss.str();
    }

    void Dump(std::ostream& os) const {
        os << Dump();
    }

    void Clear() {
        m_root.ClearValue();
        m_root.ClearAllChilds();
    }

    void SetRoot(const NodeType& root) {
        m_root = root;
    }

    void SetRoot(NodeType&& root) {
        m_root = std::move(root);
    }

   private:
    void PrintImpl(std::ostringstream& oss,
                   const std::string&  prefix,
                   const NodeType&     node,
                   bool                isRightChild) const {
        oss << prefix;
        oss << (isRightChild ? "├───" : "└───");

        node.PrintNodeData(oss);
        oss << "\n";

        if (!node.IsLeaf()) {
            std::string newPrefix = prefix + (isRightChild ? "│  " : "   ");

            const auto& childs = node.GetChilds();

            // reverse order as right nodes should be printed first for readability
            auto it = childs.crbegin();
            PrintImpl(oss, newPrefix, *it++, true);
            for (; it != childs.rend(); it++) {
                PrintImpl(oss, newPrefix, *it, false);
            }
        }
    }

    void DumpImpl(std::ostringstream& oss) const {
        std::stack<const NodeType*> stck;

        stck.push(&m_root);

        while (!stck.empty()) {
            const NodeType* current = stck.top();
            stck.pop();

            const auto& childs = current->GetChilds();

            oss << current->GetCurrentTypeIndex() << ' ';
            current->DumpNodeData(oss);
            oss << ' ' << childs.size() << ' ';

            // reverse order as left nodes should be saved first for convenience
            for (auto it = childs.crbegin(); it != childs.crend(); it++) {
                stck.push(&*it);
            }
        }
    }

    bool LoadImpl(std::istream& is) {
        if (!is.good()) {
            std::cerr << "something wrong with input stream" << std::endl;
            return false;
        }

        NodeType root;
        if (!root.LoadNodeData(is)) {
            return false;
        }

        int numChilds;
        if (!(is >> numChilds) || (numChilds < 0)) {
            std::cerr << "got invalid numbers of childs for tree node " << numChilds << std::endl;
            return false;
        }

        std::stack<NodeType*> stck;

        for (std::size_t i = 0; i < static_cast<std::size_t>(numChilds); i++) {
            stck.push(&root);
        }

        while (!stck.empty()) {
            NodeType* parent = stck.top();
            stck.pop();

            NodeType node;
            if (!node.LoadNodeData(is)) {
                return false;
            }

            // keep pointer to just inserted node for
            // after insert it's childs
            parent = &parent->AddChild(std::move(node));

            if (!(is >> numChilds) || (numChilds < 0)) {
                std::cerr << "got invalid numbers of childs for tree node " << numChilds << std::endl;
                return false;
            }

            for (std::size_t i = 0; i < static_cast<std::size_t>(numChilds); i++) {
                stck.push(parent);
            }
        }

        m_root = std::move(root);
        return true;
    }

   private:
    NodeType m_root; //!< root node of tree
};

} // namespace tree_lib

#endif // TREE_LIB_TREE_HPP
