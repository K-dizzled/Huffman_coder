#ifndef LAB_08_HUFFMAN_TREE_H
#define LAB_08_HUFFMAN_TREE_H

#include "TreeNode.h"
#include "vector"
#include "queue"
#include "string"
#include "array"

namespace h_arch {
    typedef std::priority_queue<TreeNode*, std::vector<TreeNode*>, NodeComparator> pr_queue_of_nodes;
    typedef std::array<int, 256> freq_table;
    typedef std::array<std::string, 256> code_table;

    class HuffTree {
    private:
        TreeNode *treeRoot;
        int f_table_size;

        void cTableRecursive(TreeNode *root, const std::string code, code_table &ct) const;

    public:
        const std::size_t ascii_variety = 256;

        HuffTree(freq_table pq);

        ~HuffTree();

        int get_f_table_size() const { return f_table_size; }

        std::string translate(const std::string &code) const;

        code_table createCodeTable() const;
    };
}

#endif