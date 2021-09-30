#ifndef LAB_08_TREE_NODE_H
#define LAB_08_TREE_NODE_H

#include "iostream"

namespace h_arch {
    class TreeNode {
    private:
        char Ch;
        std::size_t frequency;
        TreeNode *leftChild;
        TreeNode *rightChild;

    public:
        TreeNode(char ch, std::size_t freq, TreeNode *lC, TreeNode *rC);

        ~TreeNode();

        std::size_t getFreq() const;

        friend class HuffTree;

        friend class HuffmanArchiver;
    };

    struct NodeComparator {
        bool operator()(TreeNode *a, TreeNode *b) {
            return a->getFreq() > b->getFreq();
        }
    };
}
#endif