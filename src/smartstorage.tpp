#include "smartstorage.h"

#include <iostream>
#include <algorithm>
#include <vector>
#include <numeric>
#include <cmath>
#include <stdint.h>
#include <stdio.h>

//INCLUDE OWN FILES
#include "graph.h"
#include "rootedtree.h"
#include "general.h"

using namespace std;

template<typename T>
Smartstorage<T>::Smartstorage(const Graph& G, const vector<std::vector<int>>& BAGS, const bool& store_c, const bool& track_solution)
: G(G), bags(BAGS), store_c(store_c), track_solution(track_solution)
{

}

//Tree events
template<typename T>
void Smartstorage<T>::setup(const RootedTree& RT)
{
    validcandidates.resize(RT.N.size());
    c.resize(RT.N.size());
    if(track_solution)
    {
        p.resize(RT.N.size());
    }
}

template<typename T>
void Smartstorage<T>::discover(const int current, const RootedTree& RT)    //Prepare current node
{
    if(track_solution)
    {
        p[current-1].resize(RT.N[current-1].size());    //P
    }
}

template<typename T>
void Smartstorage<T>::finish(const int current, const RootedTree& RT)
{
    if((RT.N[current-1].size() == 1 && current != RT.root) || (current == RT.root && RT.N[current-1].size() == 0))
    {
        initialize_leaf(current,RT);
    }
    update_current(current, RT);
    //cout << bags[current-1].size() << "," << log2(validcandidates[current-1].size()) << endl;
    if(current != RT.root)
    {
        //if(validcandidates[RT.parents[current-1]-1].size() == 0)    //Check if parent was already turned on
        //    turn_on_node_storage(RT.parents[current-1],RT);
        update_to_parent(current, RT);
        //turn_off_node_storage(current,RT);  //
        //show_state(current,RT);
    }
}

template<typename T>
void Smartstorage<T>::cleanup(const RootedTree& RT)
{
    int current = RT.root;
    cout << *max_element(c[current-1].begin(), c[current-1].end()) << endl;
    cout << max_element(c[current-1].begin(), c[current-1].end())-c[current-1].begin() << endl;
}

template<typename T>
void Smartstorage<T>::update_current(const int current, const RootedTree& RT)
{
    if((RT.N[current-1].size()>2 && current != RT.root) || (current == RT.root && RT.N[current-1].size()>1))
    {
        vector<int> w(validcandidates[current-1].size());
        w[0] = 0;
        for(size_t index = 1; index<validcandidates[current-1].size(); index++)
        {
            int j = countr_zero(validcandidates[current-1][index]);
            auto it = lower_bound(validcandidates[current-1].begin(), validcandidates[current-1].begin() + index - 1, validcandidates[current-1][index] - (T(1)<<j));
            w[index] = w[it-validcandidates[current-1].begin()] + G.weights[bags[current-1][j]-1];
        }
        for(size_t index = 0; index < c[current-1].size(); index++)
            c[current-1][index] -= w[index]*(RT.N[current-1].size()-2 +(current==RT.root));
    }
}

template<typename T>
void Smartstorage<T>::update_to_parent(const int current, const RootedTree& RT)
{
    walk_virtual_path(current,RT);
}

template<typename T>
void Smartstorage<T>::walk_virtual_path(const int current, const RootedTree& RT)                            //Chosen method of updating current information to parent information
{
    int parent = RT.parents[current-1];
    //int neighbourposition = RT.neighbourIterators[parent-1] - RT.N[parent-1].begin();

    //cout << "Walking from bag " 
    //    << current << "(" << bags[current-1].size() << "(" << log2(validcandidates[current-1].size()) << ")) to bag " 
    //    << parent << "(" << bags[parent-1].size()<< ")" << endl;


    const vector<int>& source_bag = bags[current-1];
    const vector<int>& target_bag = bags[parent-1];

    int current_virtual = 1;
    int parent_virtual = 2;


    vector<vector<int>> c_virtual(2);
    vector<vector<int>> p_virtual(2);       //P
    vector<vector<T>> valid_virtual(2);

    begin_virtual_path(current, RT, c_virtual[current_virtual-1], p_virtual[current_virtual-1], valid_virtual[current_virtual-1]);

    vector<int> bag_virtual = source_bag;

    vector<int> intersection_bag;
    set_intersection(source_bag.begin(), source_bag.end(),target_bag.begin(), target_bag.end(),back_inserter(intersection_bag));

    vector<int>::const_iterator it_source_bag = source_bag.begin();
    vector<int>::const_iterator it_intersection_bag = intersection_bag.begin();

    int forgotten_so_far = 0;
    while(it_source_bag != source_bag.end() && it_intersection_bag != intersection_bag.end())
    {
        if(*it_source_bag > *it_intersection_bag)
            it_intersection_bag++;
        else
        {

            if(*it_source_bag==*it_intersection_bag)
            {
                //KEEP
                it_source_bag++;
                it_intersection_bag++;
            }
            else
            {
                //Forget *it_source_bag
                {//BEGIN FORGET BLOCK
                    auto it = std::find(bag_virtual.begin(), bag_virtual.end(), *it_source_bag);
                    bag_virtual.erase(it);

                    int i = it_source_bag-source_bag.begin()-forgotten_so_far;

                    take_virtual_step_forget(current_virtual, c_virtual, p_virtual, i, bag_virtual, valid_virtual);

                    swap(current_virtual, parent_virtual);
                }//END FORGET BLOCK
                //cout << bag_virtual.size() << " " << log2(valid_virtual[current_virtual-1].size()) << endl;
                forgotten_so_far++;
                it_source_bag++;
            }
        }
    }
    while(it_source_bag != source_bag.end())
    {
        //Forget *it_source_bag
        {//BEGIN FORGET BLOCK
            auto it = std::find(bag_virtual.begin(), bag_virtual.end(), *it_source_bag);
            bag_virtual.erase(it);

            int i = it_source_bag-source_bag.begin()-forgotten_so_far;

            take_virtual_step_forget(current_virtual, c_virtual, p_virtual, i, bag_virtual, valid_virtual);
            swap(current_virtual, parent_virtual);
        }//END FORGET BLOCK
        //cout << bag_virtual.size() << " " << log2(valid_virtual[current_virtual-1].size()) << endl;
        forgotten_so_far++;
        it_source_bag++;
    }


    //Intersection of source_bag and target_bag is reached

    //cout << "Intersection: " << intersection_bag.size() << endl;

    it_intersection_bag = intersection_bag.begin();
    vector<int>::const_iterator it_target_bag = target_bag.begin();

    int introduced_so_far = 0;
    while(it_target_bag != target_bag.end() && it_intersection_bag != intersection_bag.end())
    {
        if(*it_intersection_bag < *it_target_bag)
            it_intersection_bag++;
        else
        {
            if(*it_target_bag == *it_intersection_bag)
            {
                //SKIP
                it_target_bag++;
                it_intersection_bag++;
            }
            else
            {
                //Introduce *it_target_bag
                {//BEGIN INTRODUCE BLOCK
                    auto it = std::lower_bound(bag_virtual.begin(), bag_virtual.end(), *it_target_bag);
                    bag_virtual.insert(it, *it_target_bag);

                    int i = it_intersection_bag-intersection_bag.begin()+introduced_so_far;

                    take_virtual_step_introduce(current_virtual, c_virtual, p_virtual, i, bag_virtual, valid_virtual);
                    swap(current_virtual,parent_virtual);
                }//END INTRODUCE BLOCK
                //cout << bag_virtual.size() << " " << log2(valid_virtual[current_virtual-1].size()) << endl;
                introduced_so_far++;
                it_target_bag++;

            }
        }
    }
    while(it_target_bag != target_bag.end())        //introduce what's left
    {
        //Introduce *it_target_bag
        {//Begin Introduce Block
            auto it = lower_bound(bag_virtual.begin(), bag_virtual.end(), *it_target_bag);
            bag_virtual.insert(it, *it_target_bag);

            int i = it_intersection_bag-intersection_bag.begin()+introduced_so_far;     //Relevant bitposition

            take_virtual_step_introduce(current_virtual, c_virtual, p_virtual, i, bag_virtual, valid_virtual);
            swap(current_virtual, parent_virtual);

        }//END INTRODUCE BLOCK
        //cout << bag_virtual.size() << " " << log2(valid_virtual[current_virtual-1].size()) << endl;
        introduced_so_far++;
        it_target_bag++;
    }
    if(store_c)
        end_virtual_path(current, RT, c_virtual[current_virtual-1], p_virtual[current_virtual-1], valid_virtual[current_virtual-1]);
    else
        end_virtual_path_no_files(current, RT, c_virtual[current_virtual-1], p_virtual[current_virtual-1], valid_virtual[current_virtual-1]);
}


template<typename T>
void Smartstorage<T>::take_virtual_step_introduce(
    const int current_virtual, vector<vector<int>>& c_virtual, vector<vector<int>>& p_virtual, 
    const int i, 
    const vector<int>& bag_virtual, vector<vector<T>>& valid_virtual)
{
    int parent_virtual = (current_virtual%2) + 1;

    valid_virtual[parent_virtual-1].resize(0);
    c_virtual[parent_virtual-1].resize(0);
    if(track_solution)
    {
        p_virtual[parent_virtual-1].resize(0);
    }
    //Setup adjacencymatrix for bag_virtual
    //vector<int> adjacency_row_virtual(bag_virtual.size(),0);
    T adjacency_mask = 0;

    int k = 0;
    for(int j = 0; j < bag_virtual.size(); j++)
    {
        //if(G.adjacent(bag_virtual[i],bag_virtual[j]))       //prob quicker to walk through the neighborhood of bag_virtual[i], but uglier
        //    adjacency_row_virtual[j] = 1;
        while(k<G.N[bag_virtual[i]-1].size() && G.N[bag_virtual[i]-1][k] < bag_virtual[j])
            k++;
        if(k<G.N[bag_virtual[i]-1].size() && G.N[bag_virtual[i]-1][k] == bag_virtual[j])
        {
            //adjacency_row_virtual[j] = 1;
            adjacency_mask += (T(1) << j);
        }
    }
    size_t remember_start = 0;
    size_t remember_end = 0;

    T lowerbitmask = (T(1)<<i) - 1;

    for(size_t current_index = 0; current_index < valid_virtual[current_virtual-1].size();)
    {
        T lower = valid_virtual[current_virtual-1][current_index] & lowerbitmask;
        T higher = (valid_virtual[current_virtual-1][current_index] - lower) << 1;

        T lower_start = valid_virtual[current_virtual-1][remember_start] & lowerbitmask;
        T higher_start = (valid_virtual[current_virtual-1][remember_start] - lower_start) << 1;

        //if(valid_virtual[current_virtual-1][current_index] - valid_virtual[current_virtual-1][remember_start] < (T(1)<<i))
        //if(higher + lower - higher_start + lower_start < (T(1)<<i))
        if(higher == higher_start)
        {
            valid_virtual[parent_virtual-1].push_back(higher + lower);
            c_virtual[parent_virtual-1].push_back(c_virtual[current_virtual-1][current_index]);
            if(track_solution)
            {
                p_virtual[parent_virtual-1].push_back(p_virtual[current_virtual-1][current_index]);
            }
            remember_end = current_index;       // -,]
            current_index++;
        }
        else
        {
            for(;remember_start <= remember_end; remember_start++)
            {
                T lower_start = valid_virtual[current_virtual-1][remember_start] & lowerbitmask;
                T higher_start = (valid_virtual[current_virtual-1][remember_start] - lower_start) << 1;

                //Find a lower significant bit than i  such that (higher_start+lower_start + (T(1)<<i) - (T(1)<<j))
                
                //int j = countr_zero(higher_start + lower_start);
                
                //independentset test
                if(higher_start + lower_start == 0 ||
                    //(adjacency_row_virtual[j] == 0 &&
                    //binary_search(valid_virtual[parent_virtual-1].begin(),    //CONSIDER DIFFERENT START AND END
                    //valid_virtual[parent_virtual-1].end(),
                    //higher_start + (T(1)<<i) +lower_start - (T(1)<<j)) == true))  
                    (adjacency_mask & (higher_start + lower_start)) == 0
                )  
                {
                    valid_virtual[parent_virtual-1].push_back(higher_start + (T(1)<<i) + lower_start);
                    c_virtual[parent_virtual-1].push_back(c_virtual[current_virtual-1][remember_start]+G.weights[bag_virtual[i]-1]);
                    if(track_solution)
                    {
                        p_virtual[parent_virtual-1].push_back(p_virtual[current_virtual-1][remember_start]);
                    }
                }
            }
        }

    }

    //CASE current_index reached end of array but there are still to add.
    for(;remember_start <= remember_end; remember_start++)
    {
        T lower_start = valid_virtual[current_virtual-1][remember_start] & lowerbitmask;
        T higher_start = (valid_virtual[current_virtual-1][remember_start] - lower_start) << 1;

        //int j = countr_zero(higher_start + lower_start);

        //independentset test                       //CONSIDER BETTER START AND END
        if(higher_start + lower_start == 0 ||
            //(adjacency_row_virtual[j] == 0 &&
            //binary_search(valid_virtual[parent_virtual-1].begin(),valid_virtual[parent_virtual-1].end(),
            //        higher_start + (T(1)<<i) +lower_start - (T(1)<<j)) == true))
            (adjacency_mask & (higher_start + lower_start)) == 0
        )
        {
            valid_virtual[parent_virtual-1].push_back(higher_start + (T(1)<<i) + lower_start);
            c_virtual[parent_virtual-1].push_back(c_virtual[current_virtual-1][remember_start]+G.weights[bag_virtual[i]-1]);
            if(track_solution)
            {
                p_virtual[parent_virtual-1].push_back(p_virtual[current_virtual-1][remember_start]);
            }
        }
    }
}

template<typename T>
void Smartstorage<T>::take_virtual_step_forget(const int current_virtual, vector<vector<int>>& c_virtual,vector<vector<int>>& p_virtual, 
    const int i, 
    const vector<int>& bag_virtual, vector<vector<T>>& valid_virtual)
{
    int parent_virtual = (current_virtual%2) + 1;
    valid_virtual[parent_virtual-1].resize(0);
    c_virtual[parent_virtual-1].resize(0);
    if(track_solution)
    {
        p_virtual[parent_virtual-1].resize(0);
    }

    T lowerbitmask = (T(1)<<i) - 1;
    for(size_t current_index = 0; current_index < valid_virtual[current_virtual-1].size(); current_index++)
    {
        if(! (valid_virtual[current_virtual-1][current_index] & (T(1)<<i)) )   //If bit at i = 0, 
        {
            T lower = valid_virtual[current_virtual-1][current_index] & lowerbitmask;
            T higher = (valid_virtual[current_virtual-1][current_index] - lower) >> 1;
            
            valid_virtual[parent_virtual-1].push_back(higher + lower);

            c_virtual[parent_virtual-1].push_back(c_virtual[current_virtual-1][current_index]);
            if(track_solution)
            {
                p_virtual[parent_virtual-1].push_back(p_virtual[current_virtual-1][current_index]);    //P
            }
        }
    }
    size_t parent_index = 0;
    for(size_t current_index = 0; current_index < valid_virtual[current_virtual-1].size(); current_index++)
    {
        if(valid_virtual[current_virtual-1][current_index] & (T(1)<<i)) //If bit at i = 1
        {
            T lower = valid_virtual[parent_virtual-1][parent_index] & lowerbitmask;
            T higher = (valid_virtual[parent_virtual-1][parent_index] - lower) << 1;
            while(valid_virtual[current_virtual-1][current_index] != higher + lower + (T(1) << i))            //MAKE SURE TO SKIP SOME CURRENT_indices
            {
                parent_index++;
                lower = valid_virtual[parent_virtual-1][parent_index] & lowerbitmask;
                higher = (valid_virtual[parent_virtual-1][parent_index] - lower) << 1;
            }
            if(c_virtual[current_virtual-1][current_index] > c_virtual[parent_virtual-1][parent_index])
            {
                c_virtual[parent_virtual-1][parent_index] = c_virtual[current_virtual-1][current_index];
                if(track_solution)
                {
                    p_virtual[parent_virtual-1][parent_index] = p_virtual[current_virtual-1][current_index];        //P
                }
            } 
            parent_index++;
        } 
    }
}


template<typename T>
void Smartstorage<T>::initialize_leaf(const int current, const RootedTree& RT)
{
    int parent = current;
    //int neighbourposition = RT.neighbourIterators[parent-1] - RT.N[parent-1].begin();

    //cout << "Initializing bag " << current << "(" << bags[current-1].size() << ")" << endl;

    const vector<int>& source_bag = vector<int>();
    const vector<int>& target_bag = bags[parent-1];

    int current_virtual = 1;
    int parent_virtual = 2;


    vector<vector<int>> c_virtual(2);
    vector<vector<int>> p_virtual(2);       //P
    vector<vector<T>> valid_virtual(2);
    {///////////////////////////////////////////////////////////////// begin_virtual_path Leaf version
    valid_virtual[current_virtual-1] = vector<T>(1,T(0));
    c_virtual[current_virtual-1] = vector<int>(1,0);
    if(track_solution)
    {
        p_virtual[current_virtual-1] = vector<int>(1,0);  //P
        iota(p_virtual[current_virtual-1].begin(), p_virtual[current_virtual-1].end(), 0);              //P, initalizes p to right position, itself
    }
    }/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    vector<int> bag_virtual = source_bag;

    vector<int> intersection_bag;
    set_intersection(source_bag.begin(), source_bag.end(),target_bag.begin(), target_bag.end(),back_inserter(intersection_bag));

    //vector<int>::const_iterator it_source_bag = source_bag.begin();
    vector<int>::const_iterator it_intersection_bag = intersection_bag.begin();


    //Intersection of source_bag and target_bag is reached

    it_intersection_bag = intersection_bag.begin();
    vector<int>::const_iterator it_target_bag = target_bag.begin();

    int introduced_so_far = 0;
    while(it_target_bag != target_bag.end() && it_intersection_bag != intersection_bag.end())
    {
        if(*it_intersection_bag < *it_target_bag)
            it_intersection_bag++;
        else
        {
            if(*it_target_bag == *it_intersection_bag)
            {
                //SKIP
                it_target_bag++;
                it_intersection_bag++;
            }
            else
            {
                //Introduce *it_target_bag
                {//BEGIN INTRODUCE BLOCK
                    auto it = std::lower_bound(bag_virtual.begin(), bag_virtual.end(), *it_target_bag);
                    bag_virtual.insert(it, *it_target_bag);

                    int i = it_intersection_bag-intersection_bag.begin()+introduced_so_far;

                    take_virtual_step_introduce(current_virtual, c_virtual, p_virtual, i, bag_virtual,valid_virtual);
                    swap(current_virtual,parent_virtual);
                }//END INTRODUCE BLOCK
                introduced_so_far++;
                it_target_bag++;

            }
        }
    }
    while(it_target_bag != target_bag.end())        //introduce what's left
    {
        //Introduce *it_target_bag
        {//Begin Introduce Block
            auto it = lower_bound(bag_virtual.begin(), bag_virtual.end(), *it_target_bag);
            bag_virtual.insert(it, *it_target_bag);

            int i = it_intersection_bag-intersection_bag.begin()+introduced_so_far;     //Relevant bitposition

            take_virtual_step_introduce(current_virtual, c_virtual, p_virtual, i, bag_virtual,valid_virtual);
            swap(current_virtual, parent_virtual);

        }//END INTRODUCE BLOCK
        introduced_so_far++;
        it_target_bag++;
    }

    validcandidates[parent-1] = move(valid_virtual[current_virtual-1]);
    c[parent-1] = move(c_virtual[current_virtual-1]);
    /*
    if(track_solution)
    {
            p[parent-1][neighbourposition] = move(p_virtual[current_virtual-1]);
    }
    */
}

template<typename T>
void Smartstorage<T>::end_virtual_path(const int current, const RootedTree& RT,
        std::vector<int>& c_virtual, std::vector<int>& p_virtual, std::vector<T>& valid_virtual)
{
    int parent = RT.parents[current-1];
    int neighbourposition = RT.neighbourIterators[parent-1] - RT.N[parent-1].begin();
    //No information exists as of yet on parent
    if(
        //parent is the root and this is its first child
        (parent==RT.root && RT.neighbourIterators[parent-1] == RT.N[parent-1].begin()) || 
        //parent is not root and this is its first child(account for parent of parent neighbour)
        (parent != RT.root && 
            (RT.neighbourIterators[parent-1] == RT.N[parent-1].begin() || 
                (RT.neighbourIterators[parent-1] - RT.N[parent-1].begin() == 1 && *RT.N[parent-1].begin() == RT.parents[parent-1])))
    )
    {
        validcandidates[parent-1] = move(valid_virtual);
        c[parent-1] = move(c_virtual);

        if(track_solution)
        {
            p[parent-1][neighbourposition] = move(p_virtual);
            compress_p_to_file(parent,neighbourposition,p[parent-1][neighbourposition]);
            p[parent-1][neighbourposition] = vector<int>();
        }
    }
    //Information on parent already exists
    else
    {
        validcandidates[parent-1] = move(valid_virtual);
        c[parent-1] = move(c_virtual);
        decompress_c_from_file_and_add(parent,c[parent-1]);
        remove_c_file(parent);
        /*
        for(size_t index = 0; index < c[parent-1].size(); index++)
        {
            c[parent-1][index] += c_virtual[current_virtual-1][index];
        }*/
        if(track_solution)
        {
            p[parent-1][neighbourposition] = move(p_virtual);
            compress_p_to_file(parent,neighbourposition,p[parent-1][neighbourposition]);
            p[parent-1][neighbourposition] = vector<int>();
        }
    }
    //Dont do anything
    if(
        (parent == RT.root && RT.neighbourIterators[parent-1]+1 == RT.N[parent-1].end()) ||
        (parent != RT.root && (
            RT.neighbourIterators[parent-1]+1 == RT.N[parent-1].end() ||
            (*(RT.neighbourIterators[parent-1]+1) == RT.parents[parent-1] && RT.neighbourIterators[parent-1]+2 == RT.N[parent-1].end())
                            )
        )
    )
    {

    }
    else
    {
        compress_c_to_file(parent, c[parent-1]);
        c[parent-1] = vector<int>();
        validcandidates[parent-1] = vector<T>();
    }
}

template<typename T>
void Smartstorage<T>::begin_virtual_path(const int current, const RootedTree& RT,
        std::vector<int>& c_virtual, std::vector<int>& p_virtual, std::vector<T>& valid_virtual)
{
    valid_virtual = move(validcandidates[current-1]);   //
    validcandidates[current-1] = vector<T>();
    c_virtual = move(c[current-1]);  
    c[current-1] = vector<int>();               
    if(track_solution)
    {
        p_virtual = vector<int>(c_virtual.size(),0);  //P
        iota(p_virtual.begin(), p_virtual.end(), 0);              //P, initalizes p to right position, itself
    }
}

template<typename T>
void Smartstorage<T>::end_virtual_path_no_files(const int current, const RootedTree& RT,
    std::vector<int>& c_virtual, std::vector<int>& p_virtual, std::vector<T>& valid_virtual)
{
    int parent = RT.parents[current-1];
    int neighbourposition = RT.neighbourIterators[parent-1] - RT.N[parent-1].begin();
    //No information exists as of yet on parent
    if(
        //parent is the root and this is its first child
        (parent==RT.root && RT.neighbourIterators[parent-1] == RT.N[parent-1].begin()) || 
        //parent is not root and this is its first child(account for parent of parent neighbour)
        (parent != RT.root && 
            (RT.neighbourIterators[parent-1] == RT.N[parent-1].begin() || 
                (RT.neighbourIterators[parent-1] - RT.N[parent-1].begin() == 1 && *RT.N[parent-1].begin() == RT.parents[parent-1])))
    )
    {
        validcandidates[parent-1] = move(valid_virtual);
        c[parent-1] = move(c_virtual);
        if(track_solution)
        {
            p[parent-1][neighbourposition] = move(p_virtual);
            compress_p_to_file(parent,neighbourposition,p[parent-1][neighbourposition]);
            p[parent-1][neighbourposition] = vector<int>();
        }
    }
    //Information on parent already exists
    else
    {
        for(size_t index = 0; index < c[parent-1].size(); index++)
        {
            c[parent-1][index] += c_virtual[index];
        }
        if(track_solution)
        {
            p[parent-1][neighbourposition] = move(p_virtual);
            compress_p_to_file(parent,neighbourposition,p[parent-1][neighbourposition]);
            p[parent-1][neighbourposition] = vector<int>();
        }
    }
}
