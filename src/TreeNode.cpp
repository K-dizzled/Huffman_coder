#include "TreeNode.h"

namespace h_arch {
    TreeNode::TreeNode(char ch, std::size_t freq, TreeNode *lC, TreeNode *rC) :
            Ch(ch), frequency(freq), leftChild(lC), rightChild(rC) {}

    std::size_t TreeNode::getFreq() const {
        return frequency;
    }

    TreeNode::~TreeNode() {
        if (leftChild)
            delete leftChild;

        if (rightChild)
            delete rightChild;
    }
}