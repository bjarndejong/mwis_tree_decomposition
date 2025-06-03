#ifndef STORAGE_H_INCLUDED
#define STORAGE_H_INCLUDED

//INCLUDE STD
#include <vector>

//FORWARD DECLARE CLASSES
class Graph;
class RootedTree;

class Storage
{
public:
    std::vector<std::vector<int>> c;
    std::vector<std::vector<int>> w;
    std::vector<std::vector<std::vector<int>>> p;

    const Graph& G;
    const std::vector<std::vector<int>> bags;

    Storage(const Graph& G, const std::vector<std::vector<int>>& BAGS);

    //Operations to be called by df_traversal
    void setup(const RootedTree& RT);
    void discover(const int& current, const RootedTree& RT);                //Prepare node
    void finish(const int& current, const RootedTree& RT);                  //Initialize_leaf, update_current, update_to_parent
    void cleanup(const RootedTree& RT);                                     //Output max_element(c[root-1].begin(),c[root-1].end())

    //Part of finish routine
    void initialize_leaf(const int& current);
    void update_current(const int& current, const RootedTree& RT);          //Account for multiplicity
    void update_to_parent(const int& current, const RootedTree& RT);

    //Part of update_to_parent routine
    void walk_virtual_path(const int& current, const RootedTree& RT);

    //Part of walk_virtual_path routine
    void take_virtual_step_forget(const int& current_virtual, std::vector<std::vector<int>>& c_virtual, std::vector<std::vector<int>>& p_virtual, const int& i);
    void take_virtual_step_introduce(const int& current_virtual, std::vector<std::vector<int>>& c_virtual, std::vector<std::vector<int>>& p_virtual, const int& i, const std::vector<int>& bag_virtual);


    //Quality functions
    void density_checker() const;
    void isolated_vertices_checker() const;

    void intersection_with_parent_bag(const int& current,const RootedTree& RT) const;

};

#endif // STORAGE_H_INCLUDED
