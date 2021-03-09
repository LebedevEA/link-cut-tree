#pragma once

#include <cassert>
#include <algorithm>

template<class T>
struct Node {
    explicit Node(T val) : val(val), subtree(val) {}
    Node() = default;
    T val;
    Node* left = nullptr;
    Node* right = nullptr;
    Node* parent = nullptr;
    int size = 1;
    bool toSwap = false; // swap kids
    void push() {
        if (!toSwap) {
            return;
        }
        toSwap = false;
        swap(left, right);
        if (left) {
            left->toSwap ^= true;
        }
        if (right) {
            right->toSwap ^= true;
        }
    }
    T subtree;
    int index = -1;
};

struct Max {
    int operator()(int first, int second) {
        return std::max(first, second);
    }
};

template<class T = int, template<class = T> typename Node_ = Node, class Func = Max>
class SplayTree {
public:
    using Node = Node_<>;

    SplayTree(int& BST_index)
        : root_splay(nullptr)
        , index(-1)
        , BST_index(BST_index)
    {}

    explicit SplayTree(Node* root, int index, int& BST_index)
        : root_splay(root)
        , index(index)
        , BST_index(BST_index) {
        if (root_splay) {
            root_splay->index = index;
        }
    }

    SplayTree(Node* left, Node* right, int index, int& BST_ind)
        : SplayTree(left, index, BST_ind) {
        splay(get_last());
        root_splay->right = right;
        if (root_splay->right) {
            root_splay->right->parent = root_splay;
        }
        root_splay->index = index;
    }

    SplayTree(SplayTree& left, SplayTree& right, int index, int& BST_ind)
        : SplayTree(left.root_splay, right.root_splay, index, BST_ind)
    {}

    SplayTree split(Node* node) {
        auto [left, right] = split_impl(node);
        root_splay = left;
        return SplayTree(right, BST_index++, BST_index);
    }

    void reverse() {
        root_splay->toSwap ^= 1;
    }

//    void add(int x) {
//        if (!root_splay) {
//            root_splay = new Node(x);
//            return;
//        }
//        add(new Node(x));
//    }

    void add(Node* node) {
        initialAdd(node);
        splay(node);
    }


    int nth_max(int n) {
        return nth_max_impl(n, root_splay);
    }

    void del(int x) {
        auto pair = split_impl(find(x, root_splay));
        auto left = pair.first;
        auto right = pair.second;
        left = left->left;
        if (left) {
            left->parent = nullptr;
        }
        root_splay = merge(left, right);
    }

    Node* get_first() {
        Node* node = root_splay;
        node->push();
        while (node->left) {
            node = node->left;
            node->push();
        }
        return node;
    }

    static Node* get_first(Node* node) {
        node = get_root(node);
        node->push();
        while (node->left) {
            node = node->left;
            node->push();
        }
        return node;
    }

    Node* get_last() {
        Node* node = root_splay;
        node->push();
        while (node->right) {
            node = node->right;
            node->push();
        }
        return node;
    }

    static Node* get_last(Node* node) {
        node = get_root(node);
        node->push();
        while (node->right) {
            node = node->right;
            node->push();
        }
        return node;
    }

    static Node* get_root(Node* node) {
        while (node->parent) {
            node = node->parent;
        }
        return node;
    }

    operator bool() {
        return root_splay;
    }

private:

    static void push(Node* node) {
        if (node) {
            node->push();
        }
    }

    static void superPush(Node* node) {
        if (node->parent) {
            superPush(node->parent);
        }
        push(node);
        push(node->left);
        push(node->right);
    }

    Node* merge(Node* left, Node* right) {
        Node* node = get_last(left);
        splay(node);
        if (node) {
            node->right = right;
            if (node->right) {
                node->right->parent = node;
            }
            fixSize(node);
            return node;
        } else {
            return right;
        }
    }

    std::pair<Node*, Node*> split_impl(Node* node) {
        splay(node);
        if (node->right) {
            node->right->parent = nullptr;
            Node* tmp = node->right;
            node->right = nullptr;
            fixSize(node);
            return { node, tmp };
        } else {
            return { node, nullptr };
        }
    }

    Node* find(int x, Node* node) {
        if (node->val < x) {
            return find(x, node->right);
        } else if (node->val == x) {
            return node;
        } else {
            return find(x, node->left);
        }
    }

    int nth_max_impl(int n, Node* node) {
        if (size(node->right) > n - 1) {
            return nth_max_impl(n, node->right);
        } else if (size(node->right) == n - 1) {
            return node->val;
        } else {
            return nth_max_impl(n - size(node->right) - 1, node->left);
        }
    }

    void initialAdd(Node* newNode) {
        Node* node = root_splay;
        while (true) {
            if (newNode->val > node->val) {
                if (node->right) {
                    node = node->right;
                } else {
                    node->right = newNode;
                    node->right->parent = node;
                    break;
                }
            } else if (newNode->val < node->val) {
                if (node->left) {
                    node = node->left;
                } else {
                    node->left = newNode;
                    node->left->parent = node;
                    break;
                }
            } else {
                assert(false);
            }
        }
        while (node) {
            fixSize(node);
            node = node->parent;
        }
    }

    void splay(Node* node) {
        superPush(node);
        if (!node) {
            return;
        }
        while (true) {
            int rotateType = defineRotateType(node);
            if (rotateType == -1) {
                while (root_splay->parent) {
                    root_splay = root_splay->parent;
                }
                root_splay->index = index;
                return;
            } else if (rotateType == 0) {
                zigL(node);
            } else if (rotateType == 1) {
                zigR(node);
            } else if (rotateType == 2) {
                zigZigL(node);
            } else if (rotateType == 3) {
                zigZigR(node);
            } else if (rotateType == 4) {
                zigZagL(node);
            } else if (rotateType == 5) {
                zigZagR(node);
            } else {
                assert(false);
            }
        }
    }

    int defineRotateType(Node* node) {
        Node* parent = node->parent;
        if (!parent) {
            return -1;
        }
        if (parent->left == node) { // node is left son
            node = parent;
            parent = node->parent;
            if (!parent) {
                return 0;
            }
            if (parent->left == node) {  // node is left son
                return 2;
            } else {                     // node is right son
                return 5;
            }
        } else {                    // node is right son
            node = parent;
            parent = node->parent;
            if (!parent) {
                return 1;
            }
            if (parent->right == node) { // node is right son
                return 3;
            } else {                     // node is left son
                return 4;
            }
        }
    }

    void zigL(Node* node) {
        //    *
        //   /
        //  *
        Node* middleSubtree = node->right,
                * low = node, // initially
        * top = node->parent;
        if (top->parent) {
            if (top->parent->left == top) {
                top->parent->left = low;
            } else {
                top->parent->right = low;
            }
        }
        if (middleSubtree) {
            middleSubtree->parent = top;
        }
        low->parent = top->parent;
        top->left = middleSubtree;
        top->parent = low;
        low->right = top;
        fixSize(top);
        fixSize(low);
    }

    void zigR(Node* node) {
        //  *
        //   \
        //    *
        Node* middleSubtree = node->left,
                * low = node,
                * top = node->parent;
        if (top->parent) {
            if (top->parent->left == top) {
                top->parent->left = low;
            } else {
                top->parent->right = low;
            }
        }
        if (middleSubtree) {
            middleSubtree->parent = top;
        }
        low->parent = top->parent;
        top->right = middleSubtree;
        top->parent = low;
        low->left = top;
        fixSize(top);
        fixSize(low);
    }

    void zigZigL(Node* node) {
        //      *
        //     /
        //    *
        //   /
        //  *
        zigL(node->parent);
        zigL(node);
    }

    void zigZigR(Node* node) {
        //  *
        //   \
        //    *
        //     \
        //      *
        zigR(node->parent);
        zigR(node);
    }

    void zigZagL(Node* node) {
        //    *
        //   /
        //  *
        //   \
        //    *
        zigR(node);
        zigL(node);
    }

    void zigZagR(Node* node) {
        //  *
        //   \
        //    *
        //   /
        //  *
        zigL(node);
        zigR(node);
    }

    static int size(Node* node) {
        return node ? node->size : 0;
    }

    static void fixSize(Node* node) {
        node->size = size(node->left) + 1 + size(node->right);
    }

    Node* root_splay{};
    int index = -2;
    int& BST_index;
};

template<class T = int, template<class = T> typename Node_ = Node, class Func = Max>
class DD {
public:
    using Node = Node_<>;

    DD(DD& left, DD& right, int index, int& BST_index) // aka merge, left and right die after
        : root(merge(left.root, right.root))
        , index(index)
        , BST_index(BST_index) {
        root->index = index;
    }

    explicit DD(Node* root, int index, int& BST_index)
        : root(root)
        , index(index)
        , BST_index(BST_index) {
        if (root) {
            root->index = index;
        }
    }

    explicit DD(T val, int index, int& BST_index)
        : root(new Node(val))
        , index(index)
        , BST_index(BST_index) {
        root->index = index;
    }

    operator bool() {
        return root;
    }

    T func() {
        return root->subtree;
    }

    Node* add(T val, int after) {
        Node* left = nullptr;
        Node* right = nullptr;
        split(root, left, right, after);
        left = merge(left, new Node(val));
        root = merge(left, right);
        root->index = index;
    }

    void reverse() {
        root->toSwap ^= true;
    }

    Node* operator[](int index) { // [1 .. n]
        return byIndex(root, index);
    }

    static int indexOf(Node* node) { // [1 .. n]
        Node* tmp = node;
        while (tmp) {
            tmp->push();
            tmp = tmp->parent;
        }
        int sum = size(node->left) + 1;
        while (node) {
            if (node->parent and node->parent->right == node) {
                sum += size(node->parent->left) + 1;
            }
            node = node->parent;
        }
        return sum;
    }

    DD split(Node* node) { // this becomes left part, returns right part
        Node* left = nullptr;
        Node* right = nullptr;
        superPush(node);
        split(root, left, right, indexOf(node));
        if (right) {
            right->parent = nullptr;
        }
        if (left) {
            left->parent = nullptr;
        }
        root = left;
        return DD(right, BST_index++, BST_index);
    }

    Node* get_first() {
        Node* node = root;
        node->push();
        while (node->left) {
            node = node->left;
            node->push();
        }
        return node;
    }

    static Node* get_first(Node* node) {
        node = get_root(node);
        node->push();
        while (node->left) {
            node = node->left;
            node->push();
        }
        return node;
    }

    Node* get_last() {
        Node* node = root;
        node->push();
        while (node->right) {
            node = node->right;
            node->push();
        }
        return node;
    }

    static Node* get_last(Node* node) {
        node = get_root(node);
        node->push();
        while (node->right) {
            node = node->right;
            node->push();
        }
        return node;
    }

    static Node* get_root(Node* node) {
        while (node->parent) {
            node = node->parent;
        }
        return node;
    }

private:
    static void superPush(Node* node) {
        if (node->parent) {
            superPush(node->parent);
        }
        node->push();
    }

    static Node* merge(Node* left, Node* right) {
        if (left == nullptr) {
            return right;
        }
        if (right == nullptr) {
            return left;
        }
        left->push();
        right->push();
        if (left->rnd < right->rnd) {
            left->right = merge(left->right, right);
            left->size = size(left->left) + 1 + size(left->right);
            left->right->parent = left;
            fix(left);
            return left;
        } else  {
            right->left = merge(left, right->left);
            right->size = size(right->left) + 1 + size(right->right);
            right->left->parent = right;
            fix(right);
            return right;
        }
    }

    static void split(Node* node, Node*& left, Node*& right, int index) { // [1 .. index] and [index + 1 .. n]
        if (!node) {
            left = right = nullptr;
            return;
        }
        node->push();
        if (size(node->left) + 1 <= index) {
            split(node->right, node->right, right, index - size(node->left) - 1);
            if (node->right) {
                node->right->parent = node;
            }
            left = node;
        } else {
            split(node->left, left, node->left, index);
            if (node->left) {
                node->left->parent = node;
            }
            right = node;
        }
        if (left) {
            left->size = size(left->left) + 1 + size(left->right);
            fix(left);
        }
        if (right) {
            right->size = size(right->left) + 1 + size(right->right);
            fix(right);
        }
    }

    static int size(Node* node) {
        return node ? node->size : 0;
    }

    static Node* byIndex(Node* node, int index) {
        node->push();
        if (size(node->left) + 1 > index) {
            return byIndex(node->left, index);
        } else if (size(node->left) + 1 == index) {
            return node;
        } else {
            return byIndex(node->right, index - size(node->left) - 1);
        }
    }

    static void fix(Node* node) {
        if (!node->left and !node->right) {
            node->subtree = node->val;
        } else if (!node->left) {
            node->subtree = Func()(node->right->subtree, node->val);
        } else if (!node->right) {
            node->subtree = Func()(node->left->subtree, node->val);
        } else {
            node->subtree = Func()(node->right->subtree, node->val);
            node->subtree = Func()(node->left->subtree, node->subtree);
        }
    }

    Node* root = nullptr;
    int index;
    int& BST_index;
};