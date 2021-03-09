#pragma once

#include <random>
#include "BSTs.h"

template<class T = int, template<class = T, template<class = T> typename Node_ = Node, class Func = Max> typename BST_ = SplayTree>
class LCT { // Link-Cut Trees
    template<class U>
    struct LCTNode {
        explicit LCTNode(T val, std::mt19937& rand)
            : val(val)
            , rnd(rand())
        {}

        LCTNode(std::mt19937& rand) : rnd(rand()) {};
        T val;
        LCTNode* left = nullptr;
        LCTNode* right = nullptr;
        LCTNode* parent = nullptr;
        int size = 1;
        int rnd;
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
        int index = -1;
        T subtree;
        LCTNode<U>* up = nullptr;
    };
    using Node = LCTNode<T>;
    using BST = BST_<T, LCTNode>;

public:
    explicit LCT(int n) {
        for (int i = 0; i < n + 1; i++) {
            nodes.emplace_back(rand);
        }

        for (int i = 1; i <= n; i++) {
            nodes[i].val = i;
            nodes[i].index = i - 1;
            bst_vec.emplace_back(&nodes[i], BST_index++, BST_index);
        }
    }

    T get_max(int first, int second) {
        make_root(&nodes[first]);
        make_root(&nodes[second]);
        return BST::get_root(&nodes[first])->subtree;
    }

    void link(int first, int second) {
        make_root(&nodes[first]);
        make_root(&nodes[second]);
        nodes[first].up = &nodes[second];
    }

    void cut(int first, int second) {
        make_root(&nodes[first]);
        make_root(&nodes[second]);
        BST cur = bst_vec[BST::get_root(&nodes[second])->index];
        Node* node = &nodes[second];
        bst_vec.push_back(cur.split(node));
    }

    bool isConnected(int first, int second) {
        make_root(&nodes[first]);
        make_root(&nodes[second]);
        return BST::get_root(&nodes[second]) == BST::get_root(&nodes[first]);
    }

private:
    void special_for_philologist(int first, int second) {
        make_root(&nodes[first]);
        make_root(&nodes[second]);
    }

    void expose(Node* node) {
        Node* top = BST::get_first(node);
        while (top->up) {
            int index = BST::get_root(top->up)->index;
            bst_vec.push_back(bst_vec[index].split(top->up));
            if (bst_vec.back()) {
                bst_vec.back().get_first()->up = BST::get_last(top->up);
            }
            BST left = bst_vec[BST::get_root(top->up)->index];
            BST right = bst_vec[BST::get_root(top)->index];
            BST merged(left, right, BST_index++, BST_index);
            bst_vec.push_back(merged);
            top->up = nullptr;
            top = bst_vec.back().get_first();
        }
    }
    void make_root(Node* node) {
        expose(node);
        bst_vec.push_back(bst_vec[BST::get_root(node)->index].split(node));
        if (bst_vec.back()) {
            bst_vec.back().get_first()->up = bst_vec[BST::get_root(node)->index].get_last();
        }
        bst_vec[BST::get_root(node)->index].reverse();
    }
    std::vector<Node> nodes;
    std::vector<BST> bst_vec;

    std::mt19937 rand{(100)};

    int BST_index{};
};