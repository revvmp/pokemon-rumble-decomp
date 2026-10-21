#ifndef MSL_CPP_DETAIL_TREE_H
#define MSL_CPP_DETAIL_TREE_H

#include <algorithm>
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

inline tree_node_base* tree_leftmost(tree_node_base* x) {
    while (x->left_ != nullptr) {
        x = x->left_;
    }
    return x;
}

inline void tree_increment(tree_node_base*& pNode) {
    if (pNode->right_ != nullptr) {
        tree_node_base* pNext = pNode->right_;
        while (pNext->left_ != nullptr) {
            pNext = pNext->left_;
        }
        pNode = pNext;
    } else {
        while (pNode != pNode->parent()->left_) {
            pNode = pNode->parent();
        }
        pNode = pNode->parent();
    }
}

struct tree_root_ptr {
    tree_root_ptr(tree_node_base* pNode = nullptr) : node_(pNode) {}
    tree_node_base* node_;
};

template <typename Compare> class tree_begin_holder : private Compare {
public:
    tree_begin_holder() {}
    tree_begin_holder(const Compare& rCompare) : Compare(rCompare) {}
    Compare& first() { return *this; }
    tree_node_base*& second() { return node_; }

private:
    tree_node_base* node_;
};

template <typename T, typename Compare, typename Allocator> class tree_imp {
public:
    typedef T value_type;
    typedef unsigned long size_type;

    struct node : tree_node_base {
        T value_;
    };

    typedef typename Allocator::template rebind<node>::other NodeAllocator;
    typedef NodeAllocator node_allocator;

    class iterator {
    public:
        iterator() {}
        iterator(const iterator& rOther) : node_(rOther.node_) {}
        iterator(tree_node_base* pNode) : node_(pNode) {}
        bool operator==(const iterator& rOther) const { return node_ == rOther.node_; }
        bool operator!=(const iterator& rOther) const { return node_ != rOther.node_; }
        iterator& operator++() {
            increment();
            return *this;
        }
        T& operator*() const { return static_cast<node*>(node_)->value_; }
        T* operator->() const { return &static_cast<node*>(node_)->value_; }

        tree_node_base* node_;

    private:
        void increment() {
            if (node_->right_ != nullptr) {
                node_ = tree_leftmost(node_->right_);
            } else {
                while (node_ != node_->parent()->left_) {
                    node_ = node_->parent();
                }
                node_ = node_->parent();
            }
        }
    };
    class const_iterator {
    public:
        const_iterator() {}
        const_iterator(const iterator& rOther) : node_(rOther.node_) {}
        const_iterator(tree_node_base* pNode) : node_(pNode) {}
        const T& operator*() const { return static_cast<node*>(node_)->value_; }
        const T* operator->() const { return &static_cast<node*>(node_)->value_; }
        const_iterator& operator++() { tree_increment(node_); return *this; }
        bool operator==(const const_iterator& rOther) const { return node_ == rOther.node_; }
        bool operator!=(const const_iterator& rOther) const { return !(*this == rOther); }
        tree_node_base* node_;
    };

    tree_imp() : size_(0) { begin_node() = end_node(); }
    tree_imp(const Compare& rCompare, const Allocator& rAllocator) : size_(size_type(0), rAllocator), root_(NodeAllocator(rAllocator)), begin_(rCompare) { begin_node() = end_node(); }
    ~tree_imp() {
        if (root() != nullptr) {
            destroy(static_cast<node*>(root()));
        }
    }

    size_type size() const { return count(); }

    iterator begin() { return iterator(begin_node()); }
    iterator end() { return iterator(end_node()); }
    const_iterator begin() const { return const_iterator(const_cast<tree_imp*>(this)->begin_node()); }
    const_iterator end() const { return const_iterator(const_cast<tree_imp*>(this)->end_node()); }

    void clear() {
        if (root() != nullptr) {
            destroy(static_cast<node*>(root()));
            size_.first() = 0;
            root() = nullptr;
            begin_node() = end_node();
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

    template <typename Key> iterator find(const Key& rKey) {
        tree_node_base* pResult = end_node();
        tree_node_base* pNode = root();
        while (pNode != nullptr) {
            if (!Compare()(static_cast<node*>(pNode)->value_, rKey)) {
                pResult = pNode;
                pNode = pNode->left_;
            } else {
                pNode = pNode->right_;
            }
        }
        if (pResult == end_node() || Compare()(rKey, static_cast<node*>(pResult)->value_)) {
            pResult = end_node();
        }
        return iterator(pResult);
    }

    void erase(iterator position) { erase(const_iterator(position)); }

    void erase(const_iterator position) {
        tree_node_base* pNode = position.node_;
        if (pNode == begin_node()) {
            begin_node() = tree_next(pNode);
        }
        tree_balance_for_erase(pNode, root());
        alloc().destroy(&static_cast<node*>(pNode)->value_);
        ::operator delete(pNode);
        --size_.first();
    }

    tree_node_base* end_node() { return reinterpret_cast<tree_node_base*>(&root_.second()); }
    tree_node_base*& begin_node() { return begin_.second(); }
    tree_node_base*& root() { return *reinterpret_cast<tree_node_base**>(end_node()); }
    Allocator& alloc() { return size_.second(); }
    const Allocator& alloc() const { return size_.second(); }
    NodeAllocator& node_alloc() { return root_.first(); }
    tree_node_base* root() const { return const_cast<tree_imp*>(this)->root(); }
    size_type& sz() { return size_.first(); }
    tree_node_base*& front() { return begin_node(); }
    tree_node_base* front() const { return const_cast<tree_imp*>(this)->begin_node(); }

    template <typename Key> value_type& find_or_insert(const Key& rKey) {
        node* pPred;
        bool left;
        bool leftmost;
        tree_node_base* pParent = find_leaf(rKey, pPred, left, leftmost);
        if (pPred == nullptr || Compare()(pPred->value_, rKey)) {
            return insert_node_at(pParent, left, leftmost, value_type(rKey, typename value_type::second_type()))->value_;
        }
        return pPred->value_;
    }

    tree_node_base* find_leaf(const value_type& rValue, node*& rpPred, bool& rLeft, bool& rLeftmost);
    template <typename Key> tree_node_base* find_leaf(const Key& rKey, node*& rpPred, bool& rLeft, bool& rLeftmost);
    node* insert_node_at(tree_node_base* pParent, bool left, bool leftmost, const value_type& rValue);

    void destroy(node* pNode);
    void copy(const tree_imp& rOther);
    void copy_children(node* pDestination, node* pSource, bool left);
    void swap(tree_imp& rOther);

private:
    const size_type& count() const { return size_.first(); }

    Metrowerks::compressed_pair<size_type, Allocator> size_;
    Metrowerks::compressed_pair<node_allocator, tree_root_ptr> root_;
    tree_begin_holder<Compare> begin_;
};

template <typename T, typename Compare, typename Allocator>
tree_node_base* tree_imp<T, Compare, Allocator>::find_leaf(const value_type& rValue, node*& rpPred, bool& rLeft, bool& rLeftmost) {
    rpPred = nullptr;
    node* pLeaf = static_cast<node*>(end_node());
    node* pNode = static_cast<node*>(root());
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
template <typename Key>
tree_node_base* tree_imp<T, Compare, Allocator>::find_leaf(const Key& rKey, node*& rpPred, bool& rLeft, bool& rLeftmost) {
    rpPred = nullptr;
    node* pLeaf = static_cast<node*>(end_node());
    node* pNode = static_cast<node*>(root());
    rLeft = true;
    rLeftmost = true;
    while (pNode != nullptr) {
        pLeaf = pNode;
        if (Compare()(rKey, pNode->value_)) {
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
    if (size_.first() == size_type(-1)) {
        __throw_tree_length_error();
    }
    Metrowerks::alloc_ptr<node, node_allocator&> holder(root_.first().allocate(1), root_.first());
    alloc().construct(&holder.get()->value_, rValue);
    node* pNode = holder.release();
    pNode->right_ = nullptr;
    pNode->left_ = nullptr;
    new (&pNode->parent_) unsigned long(reinterpret_cast<unsigned long>(pParent));
    if (left) {
        pParent->left_ = pNode;
    } else {
        pParent->right_ = pNode;
    }
    ++size_.first();
    tree_balance_after_insert(pNode, root());
    if (leftmost) {
        begin_node() = pNode;
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
    alloc().destroy(&pNode->value_);
    ::operator delete(pNode);
}

template <typename T, typename Compare, typename Allocator>
void tree_imp<T, Compare, Allocator>::copy_children(node* pDestination, node* pSource, bool left) {
    NodeAllocator& nodeAllocator = node_alloc();
    Metrowerks::alloc_ptr<node, typename Allocator::template rebind<node>::other&> pNode(nodeAllocator.allocate(1), nodeAllocator);
    pNode.get()->left_ = pNode.get()->right_ = nullptr;
    pNode.get()->set_parent(pDestination);
    if (left) {
        alloc().construct(&pNode.get()->value_, static_cast<node*>(pSource->left_)->value_);
        front() = pNode.get();
        pDestination->left_ = pNode.release();
        pDestination = static_cast<node*>(pDestination->left_);
        pSource = static_cast<node*>(pSource->left_);
    } else {
        alloc().construct(&pNode.get()->value_, static_cast<node*>(pSource->right_)->value_);
        pDestination->right_ = pNode.release();
        pDestination = static_cast<node*>(pDestination->right_);
        pSource = static_cast<node*>(pSource->right_);
    }
    if ((pSource->parent_ & 1) != 0) {
        pDestination->set_red();
    } else {
        pDestination->set_black();
    }
    ++sz();
    if (pSource->right_ != nullptr) {
        copy_children(pDestination, pSource, false);
    }
    if (pSource->left_ != nullptr) {
        copy_children(pDestination, pSource, true);
    }
}

template <typename T, typename Compare, typename Allocator>
void tree_imp<T, Compare, Allocator>::copy(const tree_imp& rOther) {
    if (rOther.root() != nullptr) {
        NodeAllocator& nodeAllocator = node_alloc();
        Metrowerks::alloc_ptr<node, typename Allocator::template rebind<node>::other&> pNode(nodeAllocator.allocate(1), nodeAllocator);
        alloc().construct(&pNode.get()->value_, static_cast<node*>(rOther.root())->value_);
        tree_imp temporary(begin_.first(), alloc());
        pNode.get()->left_ = pNode.get()->right_ = nullptr;
        pNode.get()->set_parent(temporary.end_node());
        if ((rOther.root()->parent_ & 1) != 0) {
            pNode.get()->set_red();
        } else {
            pNode.get()->set_black();
        }
        temporary.root() = pNode.release();
        temporary.sz() = 1;
        temporary.front() = temporary.root();
        if (rOther.root()->right_ != nullptr) {
            temporary.copy_children(static_cast<node*>(temporary.root()), static_cast<node*>(rOther.root()), false);
        }
        if (rOther.root()->left_ != nullptr) {
            temporary.copy_children(static_cast<node*>(temporary.root()), static_cast<node*>(rOther.root()), true);
        }
        swap(temporary);
    }
}

template <typename T, typename Compare, typename Allocator>
void tree_imp<T, Compare, Allocator>::swap(tree_imp& rOther) {
    if (this == &rOther) {
        return;
    }
    Metrowerks::swap(size_, rOther.size_);
    Metrowerks::swap(root_, rOther.root_);
    std::swap(begin_, rOther.begin_);
    if (root() != nullptr) {
        root()->set_parent(end_node());
    } else {
        front() = end_node();
    }
    if (rOther.root() != nullptr) {
        rOther.root()->set_parent(rOther.end_node());
    } else {
        rOther.front() = rOther.end_node();
    }
}

template <typename T, typename Compare, typename Allocator>
class red_black_tree : public tree_imp<T, Compare, Allocator> {
    typedef tree_imp<T, Compare, Allocator> base;

public:
    typedef typename base::iterator iterator;
    typedef typename base::const_iterator const_iterator;
    red_black_tree() {}
    red_black_tree(const red_black_tree& rOther) : tree_imp<T, Compare, Allocator>() { this->copy(rOther); }
    iterator begin() { return base::begin(); }
    iterator end() { return base::end(); }
    const_iterator begin() const { return base::begin(); }
    const_iterator end() const { return base::end(); }
    template <typename Key> iterator find(const Key& rKey) { return base::find(rKey); }
    template <typename Key> T& find_or_insert(const Key& rKey) { return base::find_or_insert(rKey); }
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
