#ifndef MSL_CPP_DETAIL_TREE_H
#define MSL_CPP_DETAIL_TREE_H

#include <MSL_CPP/alloc_ptr.h>
#include <MSL_CPP/allocator.h>
#include <MSL_CPP/compressed_pair.h>
#include <stdexcept>
#include <utility>


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

static inline void __throw_tree_length_error() {
    throw length_error("tree::insert length error");
}

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

void tree_balance_after_insert(tree_node_base* x, tree_node_base* root);

void tree_balance_for_erase(tree_node_base* z, tree_node_base* root);

inline tree_node_base* tree_next(tree_node_base* x) {
    if (x->right_ != nullptr) {
        x = x->right_;
        while (x->left_ != nullptr) {
            x = x->left_;
        }
    } else {
        while (x != x->parent()->left_) {
            x = x->parent();
        }
        x = x->parent();
    }
    return x;
}

template <typename T, typename Compare, typename Allocator> class tree_imp : private Allocator {
public:
    typedef T value_type;
    typedef unsigned long size_type;

    struct node : tree_node_base {
        T value_;
    };

    class iterator {
    public:
        iterator() {}
        iterator(const iterator& rOther) : node_(rOther.node_) {}
        iterator(tree_node_base* pNode) : node_(pNode) {}

        tree_node_base* node_;
    };
    typedef iterator const_iterator;

    typedef typename Allocator::template rebind<node>::other node_allocator;

    tree_imp() : size_(0), begin_(end_node()) {}
    ~tree_imp() {
        if (root_.second() != nullptr) {
            destroy(static_cast<node*>(root_.second()));
        }
    }

    size_type size() const { return size_; }

    void clear() {
        if (root_.second() != nullptr) {
            destroy(static_cast<node*>(root_.second()));
            size_ = 0;
            root_.second() = nullptr;
            begin_ = end_node();
        }
    }

    pair<iterator, bool> insert_one(const value_type& rValue) {
        node* pPred;
        bool left;
        bool leftmost;
        tree_node_base* pParent = find_leaf(rValue, pPred, left, leftmost);
        if (pPred == nullptr || Compare()(pPred->value_, rValue)) {
            return pair<iterator, bool>(iterator(insert_node_at(pParent, left, leftmost, rValue)), true);
        }
        return pair<iterator, bool>(iterator(pPred), false);
    }

    pair<iterator, bool> insert(const value_type& rValue);

    iterator find(const value_type& rValue) {
        tree_node_base* pResult = end_node();
        tree_node_base* pNode = root_.second();
        while (pNode != nullptr) {
            if (!Compare()(static_cast<node*>(pNode)->value_, rValue)) {
                pResult = pNode;
                pNode = pNode->left_;
            } else {
                pNode = pNode->right_;
            }
        }
        if (pResult == end_node() || Compare()(rValue, static_cast<node*>(pResult)->value_)) {
            pResult = end_node();
        }
        return iterator(pResult);
    }

    void erase(iterator position) {
        tree_node_base* pNode = position.node_;
        if (pNode == begin_) {
            begin_ = tree_next(pNode);
        }
        tree_balance_for_erase(pNode, root_.second());
        this->Allocator::destroy(&static_cast<node*>(pNode)->value_);
        ::operator delete(pNode);
        --size_;
    }

    tree_node_base* end_node() { return reinterpret_cast<tree_node_base*>(&root_.second()); }

    tree_node_base* find_leaf(const value_type& rValue, node*& rpPred, bool& rLeft, bool& rLeftmost);
    node* insert_node_at(tree_node_base* pParent, bool left, bool leftmost, const value_type& rValue);

    void destroy(node* pNode);

private:
    size_type size_;
    Metrowerks::compressed_pair<node_allocator, tree_node_base*> root_;
    tree_node_base* begin_;
};

template <typename T, typename Compare, typename Allocator>
tree_node_base* tree_imp<T, Compare, Allocator>::find_leaf(const value_type& rValue, node*& rpPred, bool& rLeft, bool& rLeftmost) {
    rpPred = nullptr;
    node* pLeaf = static_cast<node*>(end_node());
    node* pNode = static_cast<node*>(root_.second());
    rLeft = true;
    rLeftmost = true;
    while (pNode != nullptr) {
        pLeaf = pNode;
        if (Compare()(rValue, pNode->value_)) {
            pNode = static_cast<node*>(pNode->left_);
            rLeft = true;
        } else {
            rpPred = pLeaf;
            pNode = static_cast<node*>(pNode->right_);
            rLeft = false;
            rLeftmost = false;
        }
    }
    return pLeaf;
}

template <typename T, typename Compare, typename Allocator>
typename tree_imp<T, Compare, Allocator>::node* tree_imp<T, Compare, Allocator>::insert_node_at(tree_node_base* pParent, bool left, bool leftmost, const value_type& rValue) {
    if (size_ == size_type(-1)) {
        __throw_tree_length_error();
    }
    Metrowerks::alloc_ptr<node, node_allocator&> holder(root_.first().allocate(1), root_.first());
    this->Allocator::construct(&holder.get()->value_, rValue);
    node* pNode = holder.release();
    pNode->right_ = nullptr;
    pNode->left_ = nullptr;
    new (&pNode->parent_) unsigned long(reinterpret_cast<unsigned long>(pParent));
    if (left) {
        pParent->left_ = pNode;
    } else {
        pParent->right_ = pNode;
    }
    ++size_;
    tree_balance_after_insert(pNode, root_.second());
    if (leftmost) {
        begin_ = pNode;
    }
    return pNode;
}

template <typename T, typename Compare, typename Allocator>
pair<typename tree_imp<T, Compare, Allocator>::iterator, bool> tree_imp<T, Compare, Allocator>::insert(const value_type& rValue) {
    node* pPred;
    bool leftmost;
    bool left;
    tree_node_base* pParent = find_leaf(rValue, pPred, left, leftmost);
    if (pPred == nullptr || Compare()(pPred->value_, rValue)) {
        return pair<iterator, bool>(iterator(insert_node_at(pParent, left, leftmost, rValue)), true);
    }
    return pair<iterator, bool>(iterator(pPred), false);
}

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
