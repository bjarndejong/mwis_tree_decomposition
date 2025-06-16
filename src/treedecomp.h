#ifndef TREEDECOMP_H_INCLUDED
#define TREEDECOMP_H_INCLUDED

//INCLUDE STD
#include <vector>
#include <string>

class TreeDecomp
{
public:
    std::vector<std::vector<int>> N;        // N[node - 1] is the neighborhood of node
    std::vector<std::vector<int>> bags;     // bags[node - 1] are the bagcontents of node sorted in ascending order
    TreeDecomp(std::string ifname);
    void print_TreeDecomp() const;
};

#endif // TREEDECOMP_H_INCLUDED
