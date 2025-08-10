#ifndef ROOTEDTREE_H_INCLUDED
#define ROOTEDTREE_H_INCLUDED

//INCLUDE STD
#include <vector>
#include <functional>

class RootedTree
{
private:
    void initialize();
public:
    const std::vector<std::vector<int>> N;
    int root;

    std::vector<int> parents;
    std::vector<std::vector<int>::const_iterator> neighbourIterators;               //REPRESENTS THE STATE OF THE ROOTED TREE

    //Order of initialization list relevant, therefore odd naming
    RootedTree(std::vector<std::vector<int>>&& ADJ, int root);              // Move constructor
    RootedTree(const std::vector<std::vector<int>>& ADJ, int root);         // Copy constructor
    RootedTree(const RootedTree& other);                                    // Copy-assign constructor

    /*
    void df_traversal(
        std::function<void(const RootedTree&)> setup,
        std::function<void(const int, const RootedTree&)> discover,
        std::function<void(const int, const RootedTree&)> finish,
        std::function<void(const RootedTree&)> cleanup
                      );
    */
   template<typename SetupFunction, typename DiscoverFunction, typename FinishFunction, typename CleanupFunction>
   void df_traversal(
        SetupFunction setup,
        DiscoverFunction discover,
        FinishFunction finish,
        CleanupFunction cleanup
                      );


    void reroot(const int newroot);

    void print() const;
};

#include "rootedtree.tpp"

#endif // ROOTEDTREE_H_INCLUDED