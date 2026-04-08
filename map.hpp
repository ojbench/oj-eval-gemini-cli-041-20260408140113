#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

#include <functional>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include "utility.hpp"

namespace sjtu {

template<class Key, class T, class Compare = std::less<Key>>
class map {
public:
    using value_type = sjtu::pair<const Key, T>;

private:
    struct NodeBase {
        NodeBase* parent;
        NodeBase* left;
        NodeBase* right;
        int height;
        NodeBase() : parent(nullptr), left(nullptr), right(nullptr), height(0) {}
    };

    struct Node : public NodeBase {
        alignas(value_type) unsigned char data[sizeof(value_type)];
        value_type* get_val() { return reinterpret_cast<value_type*>(data); }
        const value_type* get_val() const { return reinterpret_cast<const value_type*>(data); }
    };

    NodeBase* header;
    size_t node_count;
    Compare comp;

    NodeBase*& root() const { return header->parent; }
    NodeBase*& leftmost() const { return header->left; }
    NodeBase*& rightmost() const { return header->right; }

    int get_height(NodeBase* node) const {
        return node ? node->height : 0;
    }

    void update_height(NodeBase* node) {
        if (node) {
            int lh = get_height(node->left);
            int rh = get_height(node->right);
            node->height = (lh > rh ? lh : rh) + 1;
        }
    }

    int get_balance(NodeBase* node) const {
        return node ? get_height(node->left) - get_height(node->right) : 0;
    }

    void rotate_left(NodeBase* x) {
        NodeBase* y = x->right;
        x->right = y->left;
        if (y->left) y->left->parent = x;
        y->parent = x->parent;
        if (x == root()) {
            root() = y;
        } else if (x == x->parent->left) {
            x->parent->left = y;
        } else {
            x->parent->right = y;
        }
        y->left = x;
        x->parent = y;
        update_height(x);
        update_height(y);
    }

    void rotate_right(NodeBase* y) {
        NodeBase* x = y->left;
        y->left = x->right;
        if (x->right) x->right->parent = y;
        x->parent = y->parent;
        if (y == root()) {
            root() = x;
        } else if (y == y->parent->left) {
            y->parent->left = x;
        } else {
            y->parent->right = x;
        }
        x->right = y;
        y->parent = x;
        update_height(y);
        update_height(x);
    }

    void balance(NodeBase* node) {
        while (node && node != header) {
            update_height(node);
            int balance_factor = get_balance(node);
            if (balance_factor > 1) {
                if (get_balance(node->left) < 0) {
                    rotate_left(node->left);
                }
                rotate_right(node);
                node = node->parent;
            } else if (balance_factor < -1) {
                if (get_balance(node->right) > 0) {
                    rotate_right(node->right);
                }
                rotate_left(node);
                node = node->parent;
            }
            node = node->parent;
        }
    }

    static NodeBase* minimum(NodeBase* x) {
        while (x->left) x = x->left;
        return x;
    }

    static NodeBase* maximum(NodeBase* x) {
        while (x->right) x = x->right;
        return x;
    }

    void init() {
        header = new NodeBase();
        header->parent = nullptr;
        header->left = header;
        header->right = header;
        node_count = 0;
    }

    void destroy(NodeBase* node) {
        if (node) {
            destroy(node->left);
            destroy(node->right);
            static_cast<Node*>(node)->get_val()->~value_type();
            delete static_cast<Node*>(node);
        }
    }

    NodeBase* copy_tree(NodeBase* node, NodeBase* parent) {
        if (!node) return nullptr;
        Node* new_node = new Node();
        new (new_node->get_val()) value_type(*static_cast<Node*>(node)->get_val());
        new_node->height = node->height;
        new_node->parent = parent;
        new_node->left = copy_tree(node->left, new_node);
        new_node->right = copy_tree(node->right, new_node);
        return new_node;
    }

public:
    class iterator;
    class const_iterator;

    class iterator {
        friend class map;
    private:
        NodeBase* node;
        iterator(NodeBase* n) : node(n) {}
    public:
        iterator() : node(nullptr) {}
        iterator(const iterator& other) : node(other.node) {}
        iterator& operator=(const iterator& other) {
            node = other.node;
            return *this;
        }
        iterator& operator++() {
            if (node->right) {
                node = node->right;
                while (node->left) node = node->left;
            } else {
                NodeBase* p = node->parent;
                while (p && node == p->right) {
                    node = p;
                    p = p->parent;
                }
                if (node->right != p) {
                    node = p;
                }
            }
            return *this;
        }
        iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        iterator& operator--() {
            if (node->parent && node->parent->parent == node && node->left == node) {
                node = node->right;
            } else if (node->left) {
                node = node->left;
                while (node->right) node = node->right;
            } else {
                NodeBase* p = node->parent;
                while (p && node == p->left) {
                    node = p;
                    p = p->parent;
                }
                node = p;
            }
            return *this;
        }
        iterator operator--(int) {
            iterator tmp = *this;
            --(*this);
            return tmp;
        }
        value_type& operator*() const {
            return *static_cast<Node*>(node)->get_val();
        }
        value_type* operator->() const {
            return static_cast<Node*>(node)->get_val();
        }
        bool operator==(const iterator& other) const { return node == other.node; }
        bool operator!=(const iterator& other) const { return node != other.node; }
        bool operator==(const const_iterator& other) const { return node == other.node; }
        bool operator!=(const const_iterator& other) const { return node != other.node; }
    };

    class const_iterator {
        friend class map;
    private:
        const NodeBase* node;
        const_iterator(const NodeBase* n) : node(n) {}
    public:
        const_iterator() : node(nullptr) {}
        const_iterator(const const_iterator& other) : node(other.node) {}
        const_iterator(const iterator& other) : node(other.node) {}
        const_iterator& operator=(const const_iterator& other) {
            node = other.node;
            return *this;
        }
        const_iterator& operator++() {
            if (node->right) {
                node = node->right;
                while (node->left) node = node->left;
            } else {
                const NodeBase* p = node->parent;
                while (p && node == p->right) {
                    node = p;
                    p = p->parent;
                }
                if (node->right != p) {
                    node = p;
                }
            }
            return *this;
        }
        const_iterator operator++(int) {
            const_iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        const_iterator& operator--() {
            if (node->parent && node->parent->parent == node && node->left == node) {
                node = node->right;
            } else if (node->left) {
                node = node->left;
                while (node->right) node = node->right;
            } else {
                const NodeBase* p = node->parent;
                while (p && node == p->left) {
                    node = p;
                    p = p->parent;
                }
                node = p;
            }
            return *this;
        }
        const_iterator operator--(int) {
            const_iterator tmp = *this;
            --(*this);
            return tmp;
        }
        const value_type& operator*() const {
            return *static_cast<const Node*>(node)->get_val();
        }
        const value_type* operator->() const {
            return static_cast<const Node*>(node)->get_val();
        }
        bool operator==(const iterator& other) const { return node == other.node; }
        bool operator!=(const iterator& other) const { return node != other.node; }
        bool operator==(const const_iterator& other) const { return node == other.node; }
        bool operator!=(const const_iterator& other) const { return node != other.node; }
    };

    map() { init(); }
    map(const map& other) {
        init();
        if (other.root()) {
            root() = copy_tree(other.root(), header);
            leftmost() = minimum(root());
            rightmost() = maximum(root());
            node_count = other.node_count;
        }
    }
    map& operator=(const map& other) {
        if (this != &other) {
            clear();
            if (other.root()) {
                root() = copy_tree(other.root(), header);
                leftmost() = minimum(root());
                rightmost() = maximum(root());
                node_count = other.node_count;
            }
        }
        return *this;
    }
    ~map() {
        clear();
        delete header;
    }

    iterator begin() { return iterator(leftmost()); }
    const_iterator begin() const { return const_iterator(leftmost()); }
    const_iterator cbegin() const { return const_iterator(leftmost()); }
    iterator end() { return iterator(header); }
    const_iterator end() const { return const_iterator(header); }
    const_iterator cend() const { return const_iterator(header); }

    bool empty() const { return node_count == 0; }
    size_t size() const { return node_count; }

    void clear() {
        destroy(root());
        root() = nullptr;
        leftmost() = header;
        rightmost() = header;
        node_count = 0;
    }

    sjtu::pair<iterator, bool> insert(const value_type& value) {
        NodeBase* y = header;
        NodeBase* x = root();
        bool comp_res = true;
        while (x) {
            y = x;
            comp_res = comp(value.first, static_cast<Node*>(x)->get_val()->first);
            if (comp_res) {
                x = x->left;
            } else if (comp(static_cast<Node*>(x)->get_val()->first, value.first)) {
                x = x->right;
            } else {
                return sjtu::pair<iterator, bool>(iterator(x), false);
            }
        }
        Node* z = new Node();
        new (z->get_val()) value_type(value);
        z->parent = y;
        z->height = 1;
        if (y == header) {
            root() = z;
            leftmost() = z;
            rightmost() = z;
        } else if (comp_res) {
            y->left = z;
            if (y == leftmost()) leftmost() = z;
        } else {
            y->right = z;
            if (y == rightmost()) rightmost() = z;
        }
        balance(z);
        ++node_count;
        return sjtu::pair<iterator, bool>(iterator(z), true);
    }

    void erase(iterator pos) {
        NodeBase* z = pos.node;
        if (!z || z == header) return;
        NodeBase* y = z;
        NodeBase* x = nullptr;
        NodeBase* x_parent = nullptr;
        if (!y->left) {
            x = y->right;
            x_parent = y->parent;
            if (x) x->parent = y->parent;
            if (y == root()) root() = x;
            else if (y == y->parent->left) y->parent->left = x;
            else y->parent->right = x;
            if (leftmost() == z) leftmost() = z->right ? minimum(z->right) : z->parent;
            if (rightmost() == z) rightmost() = z->left ? maximum(z->left) : z->parent;
        } else if (!y->right) {
            x = y->left;
            x_parent = y->parent;
            x->parent = y->parent;
            if (y == root()) root() = x;
            else if (y == y->parent->left) y->parent->left = x;
            else y->parent->right = x;
            if (leftmost() == z) leftmost() = z->parent;
            if (rightmost() == z) rightmost() = z->parent;
        } else {
            y = minimum(z->right);
            x = y->right;
            x_parent = y->parent;
            if (y->parent != z) {
                x_parent = y->parent;
                if (x) x->parent = y->parent;
                y->parent->left = x;
                y->right = z->right;
                z->right->parent = y;
            } else {
                x_parent = y;
            }
            if (y) y->parent = z->parent;
            if (z == root()) root() = y;
            else if (z == z->parent->left) z->parent->left = y;
            else z->parent->right = y;
            y->left = z->left;
            z->left->parent = y;
            y->height = z->height;
        }
        if (leftmost() == header->parent) leftmost() = header;
        if (rightmost() == header->parent) rightmost() = header;
        balance(x_parent);
        static_cast<Node*>(z)->get_val()->~value_type();
        delete static_cast<Node*>(z);
        --node_count;
    }

    size_t count(const Key& key) const {
        return find(key) == end() ? 0 : 1;
    }

    iterator find(const Key& key) {
        NodeBase* x = root();
        while (x) {
            if (comp(key, static_cast<Node*>(x)->get_val()->first)) {
                x = x->left;
            } else if (comp(static_cast<Node*>(x)->get_val()->first, key)) {
                x = x->right;
            } else {
                return iterator(x);
            }
        }
        return end();
    }

    const_iterator find(const Key& key) const {
        NodeBase* x = root();
        while (x) {
            if (comp(key, static_cast<Node*>(x)->get_val()->first)) {
                x = x->left;
            } else if (comp(static_cast<Node*>(x)->get_val()->first, key)) {
                x = x->right;
            } else {
                return const_iterator(x);
            }
        }
        return end();
    }

    T& at(const Key& key) {
        iterator it = find(key);
        if (it == end()) throw std::out_of_range("Key not found");
        return it->second;
    }

    const T& at(const Key& key) const {
        const_iterator it = find(key);
        if (it == end()) throw std::out_of_range("Key not found");
        return it->second;
    }

    T& operator[](const Key& key) {
        iterator it = find(key);
        if (it != end()) return it->second;
        return insert(value_type(key, T())).first->second;
    }
};

}

#endif
