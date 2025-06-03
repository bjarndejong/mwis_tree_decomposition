#ifndef SOLUTION_H_INCLUDED
#define SOLUTION_H_INCLUDED

#include <vector>

//FORWARD DECLARE CLASSES

#include "smartstorage.h"

class RootedTree;

template<typename T>
class Solution
{
public:
    const int positionMWIS;
    const Smartstorage<T>& S;

    std::vector<int> choices;
    std::vector<int> MWIS;
    std::vector<int> SIZES;

    Solution(const Smartstorage<T>& S, const int& positionMWIS);

    //DF traversal functions
    void setup(const RootedTree& RT);
    void discover(const int& current, const RootedTree& RT);
    void finish(const int& current, const RootedTree& RT);
    void cleanup(const RootedTree& RT);
};

#include "solution.tpp"

#endif // SOLUTION_H_INCLUDED
