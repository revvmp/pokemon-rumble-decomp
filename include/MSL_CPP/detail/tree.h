#ifndef MSL_CPP_DETAIL_TREE_H
#define MSL_CPP_DETAIL_TREE_H

#include <MSL_CPP/allocator.h>


namespace std {

template <typename T> struct less {
    bool operator()(const T& a, const T& b) const { return a < b; }
};

namespace detail {

struct tree_node_base {
    tree_node_base* parent() const { return reinterpret_cast<tree_node_base*>(parent_ & ~1); }
    void set_parent(tree_node_base* p) { parent_ = reinterpret_cast<unsigned long>(p) | (parent_ & 1); }
    bool is_red() const { return (parent_ & 1) == 1; }
    void set_red() { parent_ |= 1; }
    void set_black() { parent_ &= ~1; }

    tree_node_base* left_;
    tree_node_base* right_;
    unsigned long parent_;
};

inline void tree_rotate_left(tree_node_base* x, tree_node_base*& root) {
    tree_node_base* y = x->right_;
    if (root == x) {
        root = y;
    }
    x->right_ = y->left_;
    if (y->left_ != nullptr) {
        y->left_->set_parent(x);
    }
    y->set_parent(x->parent());
    if (x == x->parent()->left_) {
        x->parent()->left_ = y;
    } else {
        x->parent()->right_ = y;
    }
    y->left_ = x;
    x->set_parent(y);
}

inline void tree_rotate_right(tree_node_base* x, tree_node_base*& root) {
    tree_node_base* y = x->left_;
    if (root == x) {
        root = y;
    }
    x->left_ = y->right_;
    if (y->right_ != nullptr) {
        y->right_->set_parent(x);
    }
    y->set_parent(x->parent());
    if (x == x->parent()->left_) {
        x->parent()->left_ = y;
    } else {
        x->parent()->right_ = y;
    }
    y->right_ = x;
    x->set_parent(y);
}

template <typename Node> void tree_balance_after_insert(Node* x, Node* root);

template <typename T, typename Compare, typename Allocator> class tree_imp : private Allocator {
public:
    typedef T value_type;
    typedef unsigned long size_type;

    struct node : tree_node_base {
        T value_;
    };

    class iterator {
    public:
        tree_node_base* node_;
    };
    typedef iterator const_iterator;

    tree_imp() : size_(0), root_(nullptr), begin_(reinterpret_cast<tree_node_base*>(&root_)) {}
    ~tree_imp() {
        if (root_ != nullptr) {
            destroy(static_cast<node*>(root_));
        }
    }

    size_type size() const { return size_; }

    void insert_one(const value_type& rValue) {
        node* pPred;
        bool left;
        bool leftmost;
        tree_node_base* pParent = find_leaf(rValue, pPred, left, leftmost);
        if (pPred == nullptr || Compare()(pPred->value_, rValue)) {
            insert_node_at(pParent, left, leftmost, rValue);
        }
    }

    tree_node_base* find_leaf(const value_type& rValue, node*& rpPred, bool& rLeft, bool& rLeftmost);
    node* insert_node_at(tree_node_base* pParent, bool left, bool leftmost, const value_type& rValue);

    void destroy(node* pNode);

private:
    size_type size_;
    tree_node_base* root_;
    tree_node_base* begin_;
};

template <typename T, typename Compare, typename Allocator>
void tree_imp<T, Compare, Allocator>::destroy(node* pNode) {
    if (pNode->left_ != nullptr) {
        destroy(static_cast<node*>(pNode->left_));
    }
    if (pNode->right_ != nullptr) {
        destroy(static_cast<node*>(pNode->right_));
    }
    this->Allocator::destroy(&pNode->value_);
    ::operator delete(pNode);
}

template <typename T, typename Compare, typename Allocator>
class red_black_tree : public tree_imp<T, Compare, Allocator> {
public:
    red_black_tree() {}
    ~red_black_tree() {}
};

template <typename T, typename Compare, typename Allocator> class tree : public red_black_tree<T, Compare, Allocator> {
public:
    tree() {}
    ~tree() {}
};

}
}

#endif
