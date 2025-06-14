#ifndef ROOTEDTREE_H_INCLUDED
#define ROOTEDTREE_H_INCLUDED

//INCLUDE STD
#include <vector>
#include <functional>

class RootedTree
{
public:
    const std::vector<std::vector<int>> N;
    const int number_of_nodes;
    const int root;

    std::vector<int> parents;

    std::vector<std::vector<int>::const_iterator> neighbourIterators;               //REPRESENTS THE STATE OF THE ROOTED TREE

    //Order of initialization list relevant, therefore odd naming
    RootedTree(const std::vector<std::vector<int>>& ADJ, int root);

    void df_traversal(
        std::function<void(const RootedTree&)> setup,
        std::function<void(const int&, const RootedTree&)> discover,
        std::function<void(const int&, const RootedTree&)> finish,
        std::function<void(const RootedTree&)> cleanup
                      );
};

#endif // ROOTEDTREE_H_INCLUDED
