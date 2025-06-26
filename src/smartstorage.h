#ifndef SMARTSTORAGE_H_INCLUDED
#define SMARTSTORAGE_H_INCLUDED

//INCLUDE STD
#include <vector>

//FORWARD DECLARE CLASSES
class RootedTree;
class Graph;

#include "storage.h"

template<typename T>
class Smartstorage
{
public:
    const bool track_solution;
    const bool store_c;
    std::vector<std::vector<T>> validcandidates;
    std::vector<std::vector<int>> c;
    std::vector<std::vector<std::vector<int>>> p;

    const Graph& G;
    const std::vector<std::vector<int>>& bags;

    Smartstorage(const Graph& G, const std::vector<std::vector<int>>& BAGS, const bool& store_c, const bool& track_solution);

    //Tree events
    void setup(const RootedTree& RT);
    void discover(const int current, const RootedTree& RT);
    void finish(const int current, const RootedTree& RT);
    void cleanup(const RootedTree& RT);

 
    void turn_off_node_storage(const int current, const RootedTree& RT);


    void initialize_leaf(const int current, const RootedTree& RT);                   //done
 
    void update_current(const int current, const RootedTree& RT);
    void update_to_parent(const int current, const RootedTree& RT);

    //Virtual path traversal
    void walk_virtual_path(const int current, const RootedTree& RT);   
    void take_virtual_step_forget(const int current_virtual, std::vector<std::vector<int>>& c_virtual, 
        std::vector<std::vector<int>>& p_virtual, const int i, const std::vector<int>& bag_virtual, std::vector<std::vector<T>>& valid_virtual);
    void take_virtual_step_introduce(const int current_virtual, std::vector<std::vector<int>>& c_virtual, 
        std::vector<std::vector<int>>& p_virtual, const int i, const std::vector<int>& bag_virtual, std::vector<std::vector<T>>& valid_virtual);
    void begin_virtual_path(const int current, const RootedTree& RT,
        std::vector<int>& c_virtual, std::vector<int>& p_virtual, std::vector<T>& valid_virtual);
    void end_virtual_path(const int current, const RootedTree& RT,
        std::vector<int>& c_virtual, std::vector<int>& p_virtual, std::vector<T>& valid_virtual);
    void end_virtual_path_no_files(const int current, const RootedTree& RT,
        std::vector<int>& c_virtual, std::vector<int>& p_virtual, std::vector<T>& valid_virtual);

    //Test/output functions
    void show_state(const int current, const RootedTree& RT);
};

#include "smartstorage.tpp"

#endif // SMARTSTORAGE_H_INCLUDED