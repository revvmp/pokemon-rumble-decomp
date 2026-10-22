#ifndef MSL_CPP_DETAIL_TREE_H
#define MSL_CPP_DETAIL_TREE_H

#include <algorithm>
#include <MSL_CPP/functional_base.h>
#include <MSL_CPP/alloc_ptr.h>
#include <MSL_CPP/allocator.h>
#include <MSL_CPP/compressed_pair.h>
#include <stdexcept>
#include <utility>
#include <limits>


namespace Metrowerks {
template <bool, typename T, typename F> struct select { typedef T type; };
template <typename T, typename F> struct select<false, T, F> { typedef F type; };
}

namespace std {

template <class Pointer> class __pointer_plus_bit {
public:
    __pointer_plus_bit(Pointer p = nullptr);
    __pointer_plus_bit& operator=(Pointer p);
    __pointer_plus_bit& operator=(const __pointer_plus_bit& r);
    operator Pointer() const;
    Pointer operator->() const;
    bool bit() const;
    void bit(bool value);
private:
    Pointer ptr_;
};

template <class Pointer> inline __pointer_plus_bit<Pointer>::__pointer_plus_bit(Pointer p) : ptr_(p) {}
template <class Pointer> inline __pointer_plus_bit<Pointer>& __pointer_plus_bit<Pointer>::operator=(Pointer p) {
    ptr_ = reinterpret_cast<Pointer>(reinterpret_cast<unsigned long>(p) | (reinterpret_cast<unsigned long>(ptr_) & 1));
    return *this;
}
template <class Pointer> inline __pointer_plus_bit<Pointer>& __pointer_plus_bit<Pointer>::operator=(const __pointer_plus_bit& r) {
    ptr_ = reinterpret_cast<Pointer>((reinterpret_cast<unsigned long>(r.ptr_) & ~1) | (reinterpret_cast<unsigned long>(ptr_) & 1));
    return *this;
}
template <class Pointer> inline __pointer_plus_bit<Pointer>::operator Pointer() const {
    return reinterpret_cast<Pointer>(reinterpret_cast<unsigned long>(ptr_) & ~1);
}
template <class Pointer> inline Pointer __pointer_plus_bit<Pointer>::operator->() const {
    return reinterpret_cast<Pointer>(reinterpret_cast<unsigned long>(ptr_) & ~1);
}
template <class Pointer> inline bool __pointer_plus_bit<Pointer>::bit() const { return (reinterpret_cast<unsigned long>(ptr_) & 1) != 0; }
template <class Pointer> inline void __pointer_plus_bit<Pointer>::bit(bool value) {
    if (value) {
        ptr_ = reinterpret_cast<Pointer>(reinterpret_cast<unsigned long>(ptr_) | 1);
    } else {
        ptr_ = reinterpret_cast<Pointer>(reinterpret_cast<unsigned long>(ptr_) & ~1);
    }
}

enum __tree_color { black, red };

template <typename Pointer>
class __red_black_tree
{
	typedef Pointer      pointer;
protected:

	static pointer leftmost(pointer x);
	static pointer rightmost(pointer x);
	static void increment(pointer& x);
	static void decrement(pointer& x);
	static void balance_insert(pointer x, pointer root);
	static pointer balance_erase(pointer z, pointer root);
private:
	static void rotate_left(pointer x, pointer& root);
	static void rotate_right(pointer x, pointer& root);
};

template <typename Pointer>
inline
typename __red_black_tree<Pointer>::pointer
__red_black_tree<Pointer>::leftmost(pointer x)
{
	while (x->left_ != nullptr)
		x = x->left_;
	return x;
}

template <typename Pointer>
inline
typename __red_black_tree<Pointer>::pointer
__red_black_tree<Pointer>::rightmost(pointer x)
{
	while (x->right_ != nullptr)
		x = x->right_;
	return x;
}

template <typename Pointer>
void
__red_black_tree<Pointer>::increment(pointer& x)
{
	if (x->right_ != nullptr)
		x = leftmost(x->right_);
	else
	{
		while (x != x->parent_->left_)
			x = x->parent_;
		x = x->parent_;
	}
}

template <typename Pointer>
void
__red_black_tree<Pointer>::decrement(pointer& x)
{
	if (x->left_ != nullptr)
		x = rightmost(x->left_);
	else
	{
		while (x == x->parent_->left_)
			x = x->parent_;
		x = x->parent_;
	}
}

template <typename Pointer>
void
__red_black_tree<Pointer>::balance_insert(pointer x, pointer root)
{
	x->set_color(red);
	while (x != root && x->parent_->color() == red)
	{
		if (x->parent_ == x->parent_->parent_->left_)
		{
			pointer y = x->parent_->parent_->right_;
			if (y != nullptr && y->color() == red)
			{
				x->parent_->set_color(black);
				y->set_color(black);
				x = x->parent_->parent_;
				x->set_color(red);
			}
			else
			{
				if (x == x->parent_->right_)
				{
					x = x->parent_;
					rotate_left(x, root);
				}
				x->parent_->set_color(black);
				x->parent_->parent_->set_color(red);
				rotate_right(x->parent_->parent_, root);
			}
		}
		else
		{
			pointer y = x->parent_->parent_->left_;
			if (y != nullptr && y->color() == red)
			{
				x->parent_->set_color(black);
				y->set_color(black);
				x = x->parent_->parent_;
				x->set_color(red);
			}
			else
			{
				if (x == x->parent_->left_)
				{
					x = x->parent_;
					rotate_right(x, root);
				}
				x->parent_->set_color(black);
				x->parent_->parent_->set_color(red);
				rotate_left(x->parent_->parent_, root);
			}
		}
	}
	root->set_color(black);
}

template <typename Pointer>
typename __red_black_tree<Pointer>::pointer
__red_black_tree<Pointer>::balance_erase(pointer z, pointer root)
{

	pointer result = z;
	increment(result);
	pointer y = z;
	if (z->left_ != nullptr && z->right_ != nullptr)
		y = result;
	pointer x = y->left_ != nullptr ? y->left_ : y->right_;
	pointer x_parent = y->parent_;
	if (x != nullptr)
		x->parent_ = x_parent;
	bool x_is_left_child;
	if (y == y->parent_->left_)
	{
		y->parent_->left_ = x;
		x_is_left_child = true;
	}
	else
	{
		y->parent_->right_ = x;
		x_is_left_child = false;
	}
	bool removed_black = y->color() == black;
	if (y != z)
	{
		y->parent_ = z->parent_;
		if (z == y->parent_->left_)
			y->parent_->left_ = y;
		else
			y->parent_->right_ = y;
		y->left_ = z->left_;
		if (y->left_ != nullptr)
			y->left_->parent_ = y;
		else
			x_parent = y;
		y->right_ = z->right_;
		if (y->right_ != nullptr)
			y->right_->parent_ = y;
		else
			x_parent = y;
		y->set_color(z->color());
		if (root == z)
			root = y;
	}
	else if (x == nullptr && z == root)
		removed_black = false;
	if (removed_black)
	{
		while (x != root && (x == nullptr || x->color() == black))
		{
			if (x_is_left_child)
			{
				pointer w = x_parent->right_;
				if (w->color() == red)
				{
					w->set_color(black);
					x_parent->set_color(red);
					rotate_left(x_parent, root);
					w = x_parent->right_;
				}
				if ((w->left_  == nullptr || w->left_->color()  == black) &&
					(w->right_ == nullptr || w->right_->color() == black))
				{
					w->set_color(red);
					x = x_parent;
					x_parent = x->parent_;
					x_is_left_child = x == x_parent->left_;
				}
				else
				{
					if (w->right_ == nullptr || w->right_->color() == black)
					{
						w->left_->set_color(black);
						w->set_color(red);
						rotate_right(w, root);
						w = x_parent->right_;
					}
					w->set_color(x_parent->color());
					x_parent->set_color(black);
					w->right_->set_color(black);
					rotate_left(x_parent, root);
					x = root;
				}
			}
			else
			{
				pointer w = x_parent->left_;
				if (w->color() == red)
				{
					w->set_color(black);
					x_parent->set_color(red);
					rotate_right(x_parent, root);
					w = x_parent->left_;
				}
				if ((w->left_  == nullptr || w->left_->color()  == black) &&
					(w->right_ == nullptr || w->right_->color() == black))
				{
					w->set_color(red);
					x = x_parent;
					x_parent = x->parent_;
					x_is_left_child = x == x_parent->left_;
				}
				else
				{
					if (w->left_ == nullptr || w->left_->color() == black)
					{
						w->right_->set_color(black);
						w->set_color(red);
						rotate_left(w, root);
						w = x_parent->left_;
					}
					w->set_color(x_parent->color());
					x_parent->set_color(black);
					w->left_->set_color(black);
					rotate_right(x_parent, root);
					x = root;
				}
			}
		}
		if (x != nullptr)
			x->set_color(black);
	}
	return result;
}

template <typename Pointer>
void
__red_black_tree<Pointer>::rotate_left(pointer x, pointer& root)
{
	pointer y = x->right_;
	if (root == x)
		root = y;
	x->right_ = y->left_;
	if (y->left_ != nullptr)
		y->left_->parent_ = x;
	y->parent_ = pointer(x->parent_);
	if (x == x->parent_->left_)
		x->parent_->left_ = y;
	else
		x->parent_->right_ = y;
	y->left_ = x;
	x->parent_ = y;
}

template <typename Pointer>
void
__red_black_tree<Pointer>::rotate_right(pointer x, pointer& root)
{
	pointer y = x->left_;
	if (root == x)
		root = y;
	x->left_ = y->right_;
	if (y->right_ != nullptr)
		y->right_->parent_ = x;
	y->parent_ = pointer(x->parent_);
	if (x == x->parent_->left_)
		x->parent_->left_ = y;
	else
		x->parent_->right_ = y;
	y->right_ = x;
	x->parent_ = y;
}


namespace detail {

struct tree_node_base {
    unsigned color() const { return parent_.bit(); }
    void set_color(unsigned c) { parent_.bit(bool(c)); }
    tree_node_base* parent() const { return parent_; }
    void set_parent(tree_node_base* p) { parent_ = p; }
    bool is_red() const { return parent_.bit(); }
    void set_red() { parent_.bit(true); }
    void set_black() { parent_.bit(false); }

    tree_node_base* left_;
    tree_node_base* right_;
    __pointer_plus_bit<tree_node_base*> parent_;
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
    y->set_parent(x->parent_);
    if (x == x->parent_->left_) {
        x->parent_->left_ = y;
    } else {
        x->parent_->right_ = y;
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
    y->set_parent(x->parent_);
    if (x == x->parent_->left_) {
        x->parent_->left_ = y;
    } else {
        x->parent_->right_ = y;
    }
    y->right_ = x;
    x->set_parent(y);
}



inline tree_node_base* tree_next(tree_node_base* x) {
    if (x->right_ != nullptr) {
        x = x->right_;
        while (x->left_ != nullptr) {
            x = x->left_;
        }
    } else {
        while (x != x->parent_->left_) {
            x = x->parent_;
        }
        x = x->parent_;
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
        pNode = tree_leftmost(pNode->right_);
    } else {
        while (pNode != pNode->parent_->left_) {
            pNode = pNode->parent_;
        }
        pNode = pNode->parent_;
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
    const Compare& first() const { return *this; }
    tree_node_base*& second() { return node_; }

private:
    tree_node_base* node_;
};

template <typename T, typename Compare, typename Allocator> class tree_imp : public __red_black_tree<tree_node_base*> {
    typedef __red_black_tree<tree_node_base*> algorithm_base;
public:
    typedef T value_type;
    typedef unsigned long size_type;

    struct node : tree_node_base {
        T value_;
    };

    typedef typename Allocator::template rebind<node>::other NodeAllocator;
    typedef NodeAllocator node_allocator;

    template <bool IsConst> class __generic_iterator {
    public:
        typedef typename Metrowerks::select<IsConst, const T*, T*>::type pointer;
        typedef typename Metrowerks::select<IsConst, const T&, T&>::type reference;
        __generic_iterator() {}
        __generic_iterator(const __generic_iterator<false>& rOther) : node_(rOther.node_) {}
        reference operator*() const { return node_->value_; }
        pointer operator->() const { return &node_->value_; }
        __generic_iterator& operator++() { tree_imp::algorithm_base::increment(reinterpret_cast<tree_node_base*&>(node_)); return *this; }
        friend bool operator==(const __generic_iterator& rLeft, const __generic_iterator& rRight) { return rLeft.node_ == rRight.node_; }
        friend bool operator!=(const __generic_iterator& rLeft, const __generic_iterator& rRight) { return rLeft.node_ != rRight.node_; }
        node* node_;
    private:
        explicit __generic_iterator(node* pNode) : node_(pNode) {}
        friend class __generic_iterator<true>;
        friend class tree_imp;
    };
    typedef __generic_iterator<false> iterator;
    typedef __generic_iterator<true> const_iterator;

    tree_imp() : size_(0) { begin_node() = end_node(); }
    tree_imp(const Compare& rCompare, const Allocator& rAllocator) : size_(size_type(0), rAllocator), root_(NodeAllocator(rAllocator)), begin_(rCompare) { begin_node() = end_node(); }
    ~tree_imp();

    size_type size() const { return sz(); }
    size_type max_size() const { return numeric_limits<size_type>::max(); }

    iterator begin() { return iterator(static_cast<node*>(begin_node())); }
    iterator end() { return iterator(static_cast<node*>(end_node())); }
    const_iterator begin() const { return const_iterator(static_cast<node*>(const_cast<tree_imp*>(this)->begin_node())); }
    const_iterator end() const { return const_iterator(static_cast<node*>(const_cast<tree_imp*>(this)->end_node())); }

    void clear() {
        if (root() != nullptr) {
            destroy(static_cast<node*>(root()));
            size_.first() = 0;
            root() = nullptr;
            begin_node() = end_node();
        }
    }

    pair<iterator, bool> insert_one(const value_type& rValue);
    template <typename Key> pair<iterator, bool> insert_key(const Key& rKey) {
        node* pPred;
        bool leftmost;
        bool left;
        tree_node_base* pParent = find_leaf<Key>(rKey, pPred, left, leftmost);
        if (pPred == nullptr || comp()(pPred->value_, rKey)) {
            return pair<iterator, bool>(iterator(insert_node_at(pParent, left, leftmost, rKey)), true);
        }
        return pair<iterator, bool>(iterator(pPred), false);
    }

    pair<iterator, bool> insert(const value_type& rValue);

    template <typename Key> iterator find(const Key& rKey) const;

    template <typename Key> tree_node_base* do_lower_bound(const Key& rKey, tree_node_base* pNode, tree_node_base* pResult) const {
        while (pNode != nullptr) {
            if (!comp()(static_cast<node*>(pNode)->value_, rKey)) {
                pResult = pNode;
                pNode = pNode->left_;
            } else {
                pNode = pNode->right_;
            }
        }
        return pResult;
    }

    Compare& comp() { return begin_.first(); }
    const Compare& comp() const { return begin_.first(); }

    iterator erase(iterator position) {
        tree_node_base* pNext;
        tree_node_base* pNode = position.node_;
        if (pNode == begin_node()) {
            ++position;
            begin_node() = position.node_;
        }
        pNext = algorithm_base::balance_erase(pNode, root());
        alloc().destroy(&static_cast<node*>(pNode)->value_);
        ::operator delete(pNode);
        --size_.first();
        return iterator(static_cast<node*>(pNext));
    }

    iterator erase(const_iterator position) {
        tree_node_base* pNext;
        tree_node_base* pNode = position.node_;
        if (pNode == begin_node()) {
            ++position;
            begin_node() = position.node_;
        }
        pNext = algorithm_base::balance_erase(pNode, root());
        alloc().destroy(&static_cast<node*>(pNode)->value_);
        ::operator delete(pNode);
        --size_.first();
        return iterator(static_cast<node*>(pNext));
    }

    tree_node_base* end_node() const { return reinterpret_cast<tree_node_base*>(const_cast<tree_root_ptr*>(&root_.second())); }
    tree_node_base* end_node() { return reinterpret_cast<tree_node_base*>(&root_.second()); }
    tree_node_base*& begin_node() { return begin_.second(); }
    tree_node_base*& root() { return *reinterpret_cast<tree_node_base**>(end_node()); }
    Allocator& alloc() { return size_.second(); }
    const Allocator& alloc() const { return size_.second(); }
    NodeAllocator& node_alloc() { return root_.first(); }
    tree_node_base* root() const { return *reinterpret_cast<tree_node_base**>(end_node()); }
    size_type& sz() { return size_.first(); }
    tree_node_base*& front() { return begin_node(); }
    tree_node_base* front() const { return const_cast<tree_imp*>(this)->begin_node(); }

    template <typename Key> value_type& find_or_insert(const Key& rKey);

    tree_node_base* find_leaf(const value_type& rKey, node*& rpPred, bool& rLeft, bool& rLeftmost);
    template <typename Key> tree_node_base* find_leaf(const Key& rKey, node*& rpPred, bool& rLeft, bool& rLeftmost);
    node* insert_node_at(tree_node_base* pParent, bool left, bool leftmost, const value_type& rValue);

    void destroy(node* pNode);
    void copy(const tree_imp& rOther);
    void copy_children(node* pDestination, node* pSource, bool left);
    void swap(tree_imp& rOther);

public:
    const size_type& sz() const { return size_.first(); }

private:
    Metrowerks::compressed_pair<size_type, Allocator> size_;
    Metrowerks::compressed_pair<node_allocator, tree_root_ptr> root_;
    tree_begin_holder<Compare> begin_;
    friend class __generic_iterator<false>;
    friend class __generic_iterator<true>;
};

template <typename T, typename Compare, typename Allocator>
tree_imp<T, Compare, Allocator>::~tree_imp() {
    if (root() != nullptr) {
        destroy(static_cast<node*>(root()));
    }
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
        if (comp()(rKey, pNode->value_)) {
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
tree_node_base* tree_imp<T, Compare, Allocator>::find_leaf(const value_type& rKey, node*& rpPred, bool& rLeft, bool& rLeftmost) {
    rpPred = nullptr;
    node* pLeaf = static_cast<node*>(end_node());
    node* pNode = static_cast<node*>(root());
    rLeft = true;
    rLeftmost = true;
    while (pNode != nullptr) {
        pLeaf = pNode;
        if (comp()(rKey, pNode->value_)) {
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
    if (size() == max_size()) {
        throw length_error("tree::insert length error");
    }
    Metrowerks::alloc_ptr<node, node_allocator&> holder(node_alloc().allocate(1), node_alloc());
    alloc().construct(&holder->value_, rValue);
    node* pNode = holder.release();
    pNode->right_ = nullptr;
    pNode->left_ = nullptr;
    new (&pNode->parent_) __pointer_plus_bit<tree_node_base*>(pParent);
    if (left) {
        pParent->left_ = pNode;
    } else {
        pParent->right_ = pNode;
    }
    ++sz();
    __red_black_tree<tree_node_base*>::balance_insert(pNode, root());
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
    if (pPred == nullptr || comp()(pPred->value_, rValue)) {
        return pair<iterator, bool>(iterator(insert_node_at(pParent, left, leftmost, rValue)), true);
    }
    return pair<iterator, bool>(iterator(pPred), false);
}

template <typename T, typename Compare, typename Allocator>
template <typename Key>
typename tree_imp<T, Compare, Allocator>::value_type& tree_imp<T, Compare, Allocator>::find_or_insert(const Key& rKey) {
        node* pPred;
        bool left;
        bool leftmost;
        tree_node_base* pParent = find_leaf(rKey, pPred, left, leftmost);
        if (pPred == nullptr || comp()(pPred->value_, rKey)) {
            return insert_node_at(pParent, left, leftmost, value_type(rKey, typename value_type::second_type()))->value_;
        }
        return pPred->value_;
    }

template <typename T, typename Compare, typename Allocator>
pair<typename tree_imp<T, Compare, Allocator>::iterator, bool> tree_imp<T, Compare, Allocator>::insert_one(const value_type& rValue) {
        node* pPred;
        bool left;
        bool leftmost;
        tree_node_base* pParent = find_leaf(rValue, pPred, left, leftmost);
        if (pPred == nullptr || comp()(pPred->value_, rValue)) {
            return pair<iterator, bool>(iterator(insert_node_at(pParent, left, leftmost, rValue)), true);
        }
        return pair<iterator, bool>(iterator(pPred), false);
    }

template <typename T, typename Compare, typename Allocator>
template <typename Key>
typename tree_imp<T, Compare, Allocator>::iterator tree_imp<T, Compare, Allocator>::find(const Key& rKey) const {
        tree_node_base* pResult = do_lower_bound(rKey, root(), end_node());
        if (pResult == end_node() || comp()(rKey, static_cast<node*>(pResult)->value_)) {
            return iterator(static_cast<node*>(end_node()));
        }
        return iterator(static_cast<node*>(pResult));
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
    node_alloc().deallocate(pNode, 1);
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
    if (pSource->parent_.bit()) {
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
        if (rOther.root()->parent_.bit()) {
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

template <typename T, typename Compare, typename Allocator> class tree : private red_black_tree<T, Compare, Allocator> {
    typedef red_black_tree<T, Compare, Allocator> base;
    typedef tree_imp<T, Compare, Allocator> implementation;
public:
    typedef typename base::value_type value_type;
    typedef typename base::size_type size_type;
    typedef typename base::iterator iterator;
    typedef typename base::const_iterator const_iterator;
    using base::node;
    using base::NodeAllocator;
    using base::node_allocator;
    using base::do_lower_bound;
    using base::comp;
    using base::end_node;
    using base::begin_node;
    using base::root;
    using base::alloc;
    using base::node_alloc;
    using base::sz;
    using base::front;
    using base::find_leaf;
    using base::insert_node_at;
    using base::destroy;
    using base::copy;
    using base::copy_children;
    using base::swap;
    using base::erase;
    tree() {}
    ~tree() {}
    size_type size() const { return base::sz(); }
    size_type max_size() const { return base::max_size(); }
    iterator begin() { return implementation::begin(); }
    const_iterator begin() const { return implementation::begin(); }
    iterator end() { return implementation::end(); }
    const_iterator end() const { return implementation::end(); }
    template <typename Key> iterator find(const Key& rKey) { return implementation::find(rKey); }
    template <typename Key> const_iterator find(const Key& rKey) const { return implementation::find(rKey); }
    pair<iterator, bool> insert_one(const value_type& rValue) { return base::insert_one(rValue); }
    template <typename Key> pair<iterator, bool> insert_key(const Key& rKey) { return base::insert_key(rKey); }
    pair<iterator, bool> insert(const value_type& rValue) { return base::insert(rValue); }
    template <typename Key> T& find_or_insert(const Key& rKey) { return implementation::find_or_insert(rKey); }
    void clear() { base::clear(); }
    void swap(tree& rOther) { base::swap(rOther); }
    iterator erase(const_iterator position) { return base::erase(position); }
};

}
}

#endif
