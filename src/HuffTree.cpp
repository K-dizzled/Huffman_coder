#include "HuffTree.h"

namespace h_arch {
    HuffTree::HuffTree(freq_table fT) {
        pr_queue_of_nodes pq;
        int ft_size = 0;
        for (std::size_t i = 0; i < ascii_variety; ++i) {
            if (fT[i] != 0) {
                ft_size++;
                TreeNode *node = new TreeNode(char(i), fT[i], nullptr, nullptr);
                pq.push(node);
            }
        }

        while (pq.size() != 1) {
            TreeNode *left = pq.top();
            pq.pop();
            TreeNode *right = pq.top();
            pq.pop();

            std::size_t sum = left->frequency + right->frequency;

            TreeNode *subNode = new TreeNode('\0', sum, left, right);
            pq.push(subNode);
        }
        treeRoot = pq.top();
        f_table_size = ft_size;
    }

    code_table HuffTree::createCodeTable() const {
        code_table ct;
        cTableRecursive(treeRoot, std::string(""), ct);

        return ct;
    }

    std::string HuffTree::translate(const std::string &code) const {
        std::string decoded;
        TreeNode *ptr = treeRoot;

        if (!ptr->leftChild && !ptr->rightChild) {
            decoded.reserve(code.size());
            decoded.assign(code.size(), ptr->Ch);

            return decoded;
        }

        for (auto &c : code) {
            if (c == '0') {
                if (!ptr->leftChild->leftChild && !ptr->leftChild->rightChild) {
                    decoded += ptr->leftChild->Ch;
                    ptr = treeRoot;
                } else {
                    ptr = ptr->leftChild;
                }
            } else {
                if (!ptr->rightChild->leftChild && !ptr->rightChild->rightChild) {
                    decoded += ptr->rightChild->Ch;
                    ptr = treeRoot;
                } else {
                    ptr = ptr->rightChild;
                }
            }
        }


        return decoded;
    }

    void HuffTree::cTableRecursive(TreeNode *root, const std::string code, code_table &ct) const {
        if (root == nullptr)
            return;

        if (!root->leftChild && !root->rightChild) {
            if (code != "")
                ct[(uint8_t)(root->Ch)] = code;
            else
                ct[(uint8_t)(root->Ch)] = "1";
        }

        cTableRecursive(root->leftChild, code + "0", ct);
        cTableRecursive(root->rightChild, code + "1", ct);
    }

    HuffTree::~HuffTree() {
        delete treeRoot;
    }
}