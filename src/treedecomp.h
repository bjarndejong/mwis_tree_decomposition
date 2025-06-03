#ifndef TREEDECOMP_H_INCLUDED
#define TREEDECOMP_H_INCLUDED

//INCLUDE STD
#include <vector>
#include <string>

//FORWARD DECLARE CLASSES
class Graph;
class DP;

class TreeDecomp
{
public:
    std::vector<std::vector<int>> N;//N[node - 1]:  is the Neighborhood of node
    std::vector<std::vector<int>> bags;
    TreeDecomp(std::string ifname);
    void print_TreeDecomp() const;
};

#endif // TREEDECOMP_H_INCLUDED
