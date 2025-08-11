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
    domination.resize(RT.N.size());
    c.resize(RT.N.size());
    w.resize(RT.N.size());

    number_of_neighbours_forgotten.resize(RT.N.size());
    number_of_neighbours_present.resize(RT.N.size());

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
        //cout << "Initializing " << current << endl;
        initialize_leaf(current,RT);

        //cout << "Bag:" << endl;
        //print_vector(bags[current-1]);
        //cout << "Forgotten:" << endl;
        //print_vector(number_of_neighbours_forgotten[current-1]);
        //cout << "Present:" << endl;
        //print_vector(number_of_neighbours_present[current-1]);
        //cout << "Valid:" << endl;
        //print_vector(validcandidates[current-1]);
        //cout << "Domination:" << endl;
        //print_vector(domination[current-1]);
        //cout << "c:" << endl;
        //print_vector(c[current-1]);
        //cout << "w:" << endl;
        //print_vector(w[current-1]);
        //cout << endl;
    }

    update_current(current, RT);
    //cout << "After updating " << current << endl;
    //cout << "Bag:" << endl;
    //print_vector(bags[current-1]);
    //cout << "Forgotten:" << endl;
    //print_vector(number_of_neighbours_forgotten[current-1]);
    //cout << "Present:" << endl;
    //print_vector(number_of_neighbours_present[current-1]);
    //cout << "Valid:" << endl;
    //print_vector(validcandidates[current-1]);
    //cout << "Domination:" << endl;
    //print_vector(domination[current-1]);
    //cout << "c:" << endl;
    //print_vector(c[current-1]);
    //cout << "w:" << endl;
    //print_vector(w[current-1]);
    //cout << endl;
    
    if(current != RT.root)
    {
        update_to_parent(current, RT);
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
        for(size_t index = 0; index < c[current-1].size(); index++)
            c[current-1][index] -= w[current-1][index]*(RT.N[current-1].size()-2 + (current==RT.root));
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


    const vector<int>& source_bag = bags[current-1];
    const vector<int>& target_bag = bags[parent-1];

    int current_virtual = 1;
    int parent_virtual = 2;


    vector<vector<int>> c_virtual(2);
    vector<vector<int>> w_virtual(2);
    vector<vector<int>> p_virtual(2);       //P
    vector<vector<T>> valid_virtual(2);
    vector<vector<T>> domination_virtual(2);

    vector<vector<int>> neihbours_forgotten_virtual(2);
    vector<vector<int>> neighbours_present_virtual(2);

    begin_virtual_path(current, RT, 
        valid_virtual[current_virtual-1],
        domination_virtual[current_virtual-1],
        c_virtual[current_virtual-1],
        w_virtual[current_virtual-1],
        p_virtual[current_virtual-1]);

    vector<int> bag_virtual = source_bag;
    vector<int> number_of_neighbours_forgotten_virtual = number_of_neighbours_forgotten[current-1];
    vector<int> number_of_neighbours_present_virtual = number_of_neighbours_present[current-1];

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
                    auto it = lower_bound(bag_virtual.begin(), bag_virtual.end(), *it_source_bag);
                    bag_virtual.erase(it);

                    int i = it_source_bag-source_bag.begin()-forgotten_so_far;

                    take_virtual_step_forget(current_virtual, 
                        valid_virtual,
                        domination_virtual,
                        c_virtual, 
                        w_virtual,
                        p_virtual, 
                        i, 
                        *it_source_bag,
                        bag_virtual, 
                        number_of_neighbours_forgotten_virtual, 
                        number_of_neighbours_present_virtual);
                    swap(current_virtual, parent_virtual);

                    //cout << "After forgetting " << *it_source_bag << endl;
                    //cout << "Bag:" << endl;
                    //print_vector(bag_virtual);
                    //cout << "Forgotten:" << endl;
                    //print_vector(number_of_neighbours_forgotten_virtual);
                    //cout << "Present:" << endl;
                    //print_vector(number_of_neighbours_present_virtual);
                    //cout << "Valid:" << endl;
                    //print_vector(valid_virtual[current_virtual-1]);
                    //cout << "Domination:" << endl;
                    //print_vector(domination_virtual[current_virtual-1]);
                    //cout << "c:" << endl;
                    //print_vector(c_virtual[current_virtual-1]);
                    //cout << "w:" << endl;
                    //print_vector(w_virtual[current_virtual-1]);
                    //cout << endl;
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
            auto it = lower_bound(bag_virtual.begin(), bag_virtual.end(), *it_source_bag);
            bag_virtual.erase(it);

            int i = it_source_bag-source_bag.begin()-forgotten_so_far;

            take_virtual_step_forget(current_virtual, 
                valid_virtual,
                domination_virtual,
                c_virtual, 
                w_virtual,
                p_virtual, 
                i, 
                *it_source_bag,
                bag_virtual,
                number_of_neighbours_forgotten_virtual,
                number_of_neighbours_present_virtual
            );
            swap(current_virtual, parent_virtual);
        }//END FORGET BLOCK
        forgotten_so_far++;
        it_source_bag++;
    }


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
                    auto it = lower_bound(bag_virtual.begin(), bag_virtual.end(), *it_target_bag);
                    bag_virtual.insert(it, *it_target_bag);

                    int i = it_intersection_bag-intersection_bag.begin()+introduced_so_far;

                    take_virtual_step_introduce(current_virtual, 
                        valid_virtual,
                        domination_virtual,
                        c_virtual, 
                        w_virtual,
                        p_virtual, 
                        i, 
                        bag_virtual, 
                        number_of_neighbours_forgotten_virtual,
                        number_of_neighbours_present_virtual
                    );
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

            take_virtual_step_introduce(current_virtual, 
                valid_virtual,
                domination_virtual,
                c_virtual,
                w_virtual,
                p_virtual, 
                i, 
                bag_virtual,
                number_of_neighbours_forgotten_virtual,
                number_of_neighbours_present_virtual 
            );
            swap(current_virtual, parent_virtual);

        }//END INTRODUCE BLOCK
        introduced_so_far++;
        it_target_bag++;
    }
    //if(store_c)
    //    end_virtual_path(current, RT, c_virtual[current_virtual-1], p_virtual[current_virtual-1], valid_virtual[current_virtual-1]);
    //else
    end_virtual_path_no_files(current, RT, 
        valid_virtual[current_virtual-1],
        domination_virtual[current_virtual-1],
        c_virtual[current_virtual-1],
        w_virtual[current_virtual-1], 
        p_virtual[current_virtual-1],
        number_of_neighbours_forgotten_virtual,
        number_of_neighbours_present_virtual
    );
}


template<typename T>
void Smartstorage<T>::take_virtual_step_introduce(
    const int current_virtual, 
    vector<vector<T>>& valid_virtual,
    vector<vector<T>>& domination_virtual,
    vector<vector<int>>& c_virtual, 
    vector<vector<int>>& w_virtual,
    vector<vector<int>>& p_virtual, 
    const int i,
    const vector<int>& bag_virtual,
    vector<int>& number_of_neighbours_forgotten_virtual,
    vector<int>& number_of_neighbours_present_virtual
    )
{
    int parent_virtual = (current_virtual%2) + 1;

    valid_virtual[parent_virtual-1].resize(0);
    domination_virtual[parent_virtual-1].resize(0);
    c_virtual[parent_virtual-1].resize(0);
    w_virtual[parent_virtual-1].resize(0);

    if(track_solution)
    {
        p_virtual[parent_virtual-1].resize(0);
    }

    number_of_neighbours_forgotten_virtual.insert(number_of_neighbours_forgotten_virtual.begin()+i, 0);
    number_of_neighbours_present_virtual.insert(number_of_neighbours_present_virtual.begin()+i, 0);

    T adjacency_mask = 0;
    T all_neighbours_accounted_mask = 0;

    int k = 0;
    for(int j = 0; j < bag_virtual.size(); j++)
    {
        while(k<G.N[bag_virtual[i]-1].size() && G.N[bag_virtual[i]-1][k] < bag_virtual[j])
            k++;
        if(k<G.N[bag_virtual[i]-1].size() && G.N[bag_virtual[i]-1][k] == bag_virtual[j])
        {
            adjacency_mask |= (T(1) << j);
            number_of_neighbours_present_virtual[i]++;
            number_of_neighbours_present_virtual[j]++;
        }
        // Moved it outside of above if statement
        if(number_of_neighbours_forgotten_virtual[j] + number_of_neighbours_present_virtual[j] ==  G.N[bag_virtual[j]-1].size())
            all_neighbours_accounted_mask |= (T(1) << j);
    }
    // number_of_neighbours_present[i] only fully updated after loop(doing it extra for i in loop is fine)
    if(number_of_neighbours_forgotten_virtual[i] + number_of_neighbours_present_virtual[i] ==  G.N[bag_virtual[i]-1].size())
        all_neighbours_accounted_mask |= (T(1) << i);

    size_t remember_start = 0;  //current_index_zero,
    size_t remember_end = 0;    //current_index_one

    T lowerbitmask = (T(1)<<i) - 1;

    for(size_t current_index = 0; current_index < valid_virtual[current_virtual-1].size();)
    {
        T lower = valid_virtual[current_virtual-1][current_index] & lowerbitmask;
        T higher = (valid_virtual[current_virtual-1][current_index] - lower) << 1;

        T lower_start = valid_virtual[current_virtual-1][remember_start] & lowerbitmask;
        T higher_start = (valid_virtual[current_virtual-1][remember_start] - lower_start) << 1;

        if(higher == higher_start)
        {
            T domination_lower = domination_virtual[current_virtual-1][current_index] & lowerbitmask;
            T domination_higher = (domination_virtual[current_virtual-1][current_index] - domination_lower) << 1;

            bool domination_status = (T((lower+higher) & adjacency_mask) != 0);
            if(((domination_higher + (T(domination_status)<< i) + domination_lower) & all_neighbours_accounted_mask) == all_neighbours_accounted_mask)
            {
                valid_virtual[parent_virtual-1].push_back(higher | lower);
                domination_virtual[parent_virtual-1].push_back(domination_higher + (T(domination_status)<< i) + domination_lower);
                c_virtual[parent_virtual-1].push_back(c_virtual[current_virtual-1][current_index]);
                w_virtual[parent_virtual-1].push_back(w_virtual[current_virtual-1][current_index]);
                if(track_solution)
                {
                    p_virtual[parent_virtual-1].push_back(p_virtual[current_virtual-1][current_index]);
                }
            }
            remember_end = current_index;
            current_index++;
        }
        else
        {
            for(;remember_start <= remember_end; remember_start++)
            {
                T lower_start = valid_virtual[current_virtual-1][remember_start] & lowerbitmask;
                T higher_start = (valid_virtual[current_virtual-1][remember_start] - lower_start) << 1;

                T domination_lower = domination_virtual[current_virtual-1][remember_start] & lowerbitmask;
                T domination_higher = (domination_virtual[current_virtual-1][remember_start] - domination_lower) << 1;

                if((adjacency_mask & (higher_start | lower_start)) == 0)  // Where is the domination check?
                {
                    valid_virtual[parent_virtual-1].push_back(higher_start | (T(1)<<i) | lower_start);
                    domination_virtual[parent_virtual-1].push_back((domination_higher + (T(1)<<i) + domination_lower) | adjacency_mask);
                    c_virtual[parent_virtual-1].push_back(c_virtual[current_virtual-1][remember_start] + G.weights[bag_virtual[i]-1]);
                    w_virtual[parent_virtual-1].push_back(w_virtual[current_virtual-1][remember_start] + G.weights[bag_virtual[i]-1]);
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

        T domination_lower = domination_virtual[current_virtual-1][remember_start] & lowerbitmask;
        T domination_higher = (domination_virtual[current_virtual-1][remember_start] - domination_lower) << 1;

        if((adjacency_mask & (higher_start | lower_start)) == 0)
        {
            valid_virtual[parent_virtual-1].push_back(higher_start | (T(1)<<i) | lower_start);
            domination_virtual[parent_virtual-1].push_back((domination_higher + (T(1)<<i) + domination_lower) | adjacency_mask);
            c_virtual[parent_virtual-1].push_back(c_virtual[current_virtual-1][remember_start]+G.weights[bag_virtual[i]-1]);
            w_virtual[parent_virtual-1].push_back(w_virtual[current_virtual-1][remember_start]+G.weights[bag_virtual[i]-1]);
            if(track_solution)
            {
                p_virtual[parent_virtual-1].push_back(p_virtual[current_virtual-1][remember_start]);
            }
        }
    }
}

template<typename T>
void Smartstorage<T>::take_virtual_step_forget(const int current_virtual, 
    vector<vector<T>>& valid_virtual,
    vector<vector<T>>& domination_virtual,
    vector<vector<int>>& c_virtual,
    vector<vector<int>>& w_virtual,
    vector<vector<int>>& p_virtual, 
    const int i, 
    const int forgotten_vertex,
    const vector<int>& bag_virtual,
    vector<int>& number_of_neighbours_forgotten_virtual,
    vector<int>& number_of_neighbours_present_virtual)
{
    int parent_virtual = (current_virtual%2) + 1;

    valid_virtual[parent_virtual-1].resize(0);
    domination_virtual[parent_virtual-1].resize(0);
    c_virtual[parent_virtual-1].resize(0);
    w_virtual[parent_virtual-1].resize(0);
    if(track_solution)
    {
        p_virtual[parent_virtual-1].resize(0);
    }
    //cout << "I'm forgetting " << forgotten_vertex <<endl;
    number_of_neighbours_forgotten_virtual.erase(number_of_neighbours_forgotten_virtual.begin()+i);
    number_of_neighbours_present_virtual.erase(number_of_neighbours_present_virtual.begin()+i);

    int k = 0;
    for(int j = 0; j < bag_virtual.size(); j++)
    {
        while(k<G.N[forgotten_vertex-1].size() && G.N[forgotten_vertex-1][k] < bag_virtual[j])
            k++;
        if(k<G.N[forgotten_vertex-1].size() && G.N[forgotten_vertex-1][k] == bag_virtual[j])
        {
            number_of_neighbours_forgotten_virtual[j]++;
            number_of_neighbours_present_virtual[j]--;
        }
    }
    
    //print_vector(number_of_neighbours_forgotten_virtual);

    T lowerbitmask = (T(1)<<i) - 1;
    // Do two walks through the vector simultaneously, similar as to a merge.

    size_t current_index_zero = 0;
    while(current_index_zero<valid_virtual[current_virtual-1].size() && ((valid_virtual[current_virtual-1][current_index_zero] & (T(1)<<i)) != 0))
        current_index_zero++;
    size_t current_index_one = 0;
    while(current_index_one<valid_virtual[current_virtual-1].size() && ((valid_virtual[current_virtual-1][current_index_one] & (T(1)<<i)) == 0))
        current_index_one++;

    while(current_index_zero < valid_virtual[current_virtual-1].size() && current_index_one < valid_virtual[current_virtual-1].size())
    {
        T lower_zero = valid_virtual[current_virtual-1][current_index_zero] & lowerbitmask;
        T higher_zero = (valid_virtual[current_virtual-1][current_index_zero] - lower_zero) >> 1;
        T lower_one = valid_virtual[current_virtual-1][current_index_one] & lowerbitmask;
        T higher_one = (valid_virtual[current_virtual-1][current_index_one] - lower_one - (T(1)<<i)) >> 1;
        if(higher_zero <= higher_one && lower_zero <= lower_one)
        {
            valid_virtual[parent_virtual-1].push_back(higher_zero + lower_zero);
            if(track_solution)
            {
                p_virtual[parent_virtual-1].push_back(p_virtual[current_virtual-1][current_index_zero]);
            }
            T lower_zero_domination = domination_virtual[current_virtual-1][current_index_zero] & lowerbitmask;
            bool lower_domination_status = ((domination_virtual[current_virtual-1][current_index_zero] & (T(1) << i)) != 0); 
            T higher_zero_domination = (domination_virtual[current_virtual-1][current_index_zero] - lower_zero_domination - (T(lower_domination_status) << i)) >> 1;
            domination_virtual[parent_virtual-1].push_back(higher_zero_domination + lower_zero_domination);
            c_virtual[parent_virtual-1].push_back(c_virtual[current_virtual-1][current_index_zero]);
            w_virtual[parent_virtual-1].push_back(w_virtual[current_virtual-1][current_index_zero]);
            if(higher_zero == higher_one && lower_zero == lower_one)
            {
                if(c_virtual[current_virtual-1][current_index_one] > c_virtual[current_virtual-1][current_index_zero])
                {
                    T lower_one_domination = domination_virtual[current_virtual-1][current_index_one] & lowerbitmask;
                    T higher_one_domination = (domination_virtual[current_virtual-1][current_index_one] - lower_one_domination - (T(1)<<i)) >> 1;
                    valid_virtual[parent_virtual-1].back() = higher_one + lower_one;
                    if(track_solution)
                    {
                        p_virtual[parent_virtual-1].back() = p_virtual[current_virtual-1][current_index_one];
                    }
                    domination_virtual[parent_virtual-1].back() = higher_one_domination + lower_one_domination;
                    c_virtual[parent_virtual-1].back() = c_virtual[current_virtual-1][current_index_one];
                    // w. Needn't update w
                }
                current_index_one++;
                while(current_index_one<valid_virtual[current_virtual-1].size() && ((valid_virtual[current_virtual-1][current_index_one] & (T(1)<<i)) == 0))
                    current_index_one++;
            }
            current_index_zero++;
            while(current_index_zero<valid_virtual[current_virtual-1].size() && ((valid_virtual[current_virtual-1][current_index_zero] & (T(1)<<i)) != 0))
                current_index_zero++;
        }
        else
        {
            valid_virtual[parent_virtual-1].push_back(higher_one + lower_one);
            if(track_solution)
            {
                p_virtual[parent_virtual-1].push_back(p_virtual[current_virtual-1][current_index_one]);
            }
            T lower_one_domination = domination_virtual[current_virtual-1][current_index_one] & lowerbitmask;
            T higher_one_domination = (domination_virtual[current_virtual-1][current_index_one] - lower_one_domination - (T(1)<<i)) >> 1;
            domination_virtual[parent_virtual-1].push_back(higher_one_domination + lower_one_domination);
            c_virtual[parent_virtual-1].push_back(c_virtual[current_virtual-1][current_index_one]);
            w_virtual[parent_virtual-1].push_back(w_virtual[current_virtual-1][current_index_one] - G.weights[forgotten_vertex-1]);
            current_index_one++;
            while(current_index_one<valid_virtual[current_virtual-1].size() && ((valid_virtual[current_virtual-1][current_index_one] & (T(1)<<i)) == 0))
                current_index_one++;
        }
        
    }
    // Add remainder
    while(current_index_zero < valid_virtual[current_virtual-1].size())
    {
        T lower_zero = valid_virtual[current_virtual-1][current_index_zero] & lowerbitmask;
        T higher_zero = (valid_virtual[current_virtual-1][current_index_zero] - lower_zero) >> 1;
        valid_virtual[parent_virtual-1].push_back(higher_zero + lower_zero);
        if(track_solution)
        {
            p_virtual[parent_virtual-1].push_back(p_virtual[current_virtual-1][current_index_zero]);
        }
        T lower_zero_domination = domination_virtual[current_virtual-1][current_index_zero] & lowerbitmask;
        bool lower_domination_status = ((domination_virtual[current_virtual-1][current_index_zero] & (T(1) << i)) != 0); 
        T higher_zero_domination = (domination_virtual[current_virtual-1][current_index_zero] - lower_zero_domination - (T(lower_domination_status)<<i)) >> 1;
        domination_virtual[parent_virtual-1].push_back(higher_zero_domination + lower_zero_domination);
        c_virtual[parent_virtual-1].push_back(c_virtual[current_virtual-1][current_index_zero]);
        w_virtual[parent_virtual-1].push_back(w_virtual[current_virtual-1][current_index_zero]);
        current_index_zero++;
        while(current_index_zero<valid_virtual[current_virtual-1].size() && ((valid_virtual[current_virtual-1][current_index_zero] & (T(1)<<i)) != 0))
            current_index_zero++;
    }
    while(current_index_one < valid_virtual[current_virtual-1].size())
    {
        T lower_one = valid_virtual[current_virtual-1][current_index_one] & lowerbitmask;
        T higher_one = (valid_virtual[current_virtual-1][current_index_one] - lower_one - (T(1)<<i)) >> 1;
        valid_virtual[parent_virtual-1].push_back(higher_one + lower_one);
        if(track_solution)
        {
            p_virtual[parent_virtual-1].push_back(p_virtual[current_virtual-1][current_index_one]);
        }
        T lower_one_domination = domination_virtual[current_virtual-1][current_index_one] & lowerbitmask;
        T higher_one_domination = (domination_virtual[current_virtual-1][current_index_one] - lower_one_domination - (T(1)<<i)) >> 1;
        domination_virtual[parent_virtual-1].push_back(higher_one_domination + lower_one_domination);
        c_virtual[parent_virtual-1].push_back(c_virtual[current_virtual-1][current_index_one]);
        w_virtual[parent_virtual-1].push_back(w_virtual[current_virtual-1][current_index_one] - G.weights[forgotten_vertex-1]);
        current_index_one++;
        while(current_index_one<valid_virtual[current_virtual-1].size() && ((valid_virtual[current_virtual-1][current_index_one] & (T(1)<<i)) == 0))
            current_index_one++;
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

    vector<vector<T>> valid_virtual(2);
    vector<vector<T>> domination_virtual(2);

    vector<vector<int>> c_virtual(2);
    vector<vector<int>> w_virtual(2);
    vector<vector<int>> p_virtual(2);       //P

    {///////////////////////////////////////////////////////////////// begin_virtual_path Leaf version
    valid_virtual[current_virtual-1] = vector<T>(1,T(0));
    domination_virtual[current_virtual-1] = vector<T>(1,T(0));

    c_virtual[current_virtual-1] = vector<int>(1,0);
    w_virtual[current_virtual-1] = vector<int>(1,0);
    if(track_solution)
    {
        p_virtual[current_virtual-1] = vector<int>(1,0);  //P
        iota(p_virtual[current_virtual-1].begin(), p_virtual[current_virtual-1].end(), 0);              //P, initalizes p to right position, itself
    }
    }/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    vector<int> bag_virtual = source_bag;
    vector<int> number_of_neighbours_forgotten_virtual = vector<int>();
    vector<int> number_of_neighbours_present_virtual = vector<int>();
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
                    auto it = lower_bound(bag_virtual.begin(), bag_virtual.end(), *it_target_bag);
                    bag_virtual.insert(it, *it_target_bag);

                    int i = it_intersection_bag-intersection_bag.begin()+introduced_so_far;

                    take_virtual_step_introduce(current_virtual,
                        valid_virtual,
                        domination_virtual,
                        c_virtual, 
                        w_virtual,
                        p_virtual, 
                        i, 
                        bag_virtual,
                        number_of_neighbours_forgotten_virtual,
                        number_of_neighbours_present_virtual
                    );
                    swap(current_virtual,parent_virtual);
                }//END INTRODUCE BLOCK
                introduced_so_far++;
                it_target_bag++;

            }
        }
        print_vector(bag_virtual);
        cout << endl;
    }
    while(it_target_bag != target_bag.end())        //introduce what's left
    {
        //Introduce *it_target_bag
        {//Begin Introduce Block
            auto it = lower_bound(bag_virtual.begin(), bag_virtual.end(), *it_target_bag);
            bag_virtual.insert(it, *it_target_bag);

            int i = it_intersection_bag-intersection_bag.begin()+introduced_so_far;     //Relevant bitposition

            take_virtual_step_introduce(current_virtual, 
                valid_virtual,
                domination_virtual,
                c_virtual, 
                w_virtual,
                p_virtual, 
                i, 
                bag_virtual,
                number_of_neighbours_forgotten_virtual,
                number_of_neighbours_present_virtual
            );
            swap(current_virtual, parent_virtual);

        }//END INTRODUCE BLOCK
        introduced_so_far++;
        it_target_bag++;
    }

    validcandidates[parent-1] = move(valid_virtual[current_virtual-1]);
    domination[parent-1] = move(domination_virtual[current_virtual-1]);
    c[parent-1] = move(c_virtual[current_virtual-1]);
    w[parent-1] = move(w_virtual[current_virtual-1]);

    number_of_neighbours_forgotten[parent-1] = move(number_of_neighbours_forgotten_virtual);
    number_of_neighbours_present[parent-1] = move(number_of_neighbours_present_virtual);
    /*
    if(track_solution)
    {
            p[parent-1][neighbourposition] = move(p_virtual[current_virtual-1]);
    }
    */
}

template<typename T>
void Smartstorage<T>::end_virtual_path(const int current, const RootedTree& RT,
        vector<int>& c_virtual, vector<int>& p_virtual, vector<T>& valid_virtual)
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
    vector<T>& valid_virtual,
    vector<T>& domination_virtual,
    vector<int>& c_virtual,
    vector<int>& w_virtual, 
    vector<int>& p_virtual)
{
    valid_virtual = move(validcandidates[current-1]);
    validcandidates[current-1] = vector<T>();   // Free space just in case
    domination_virtual = move(domination[current-1]);
    domination[current-1] = vector<T>();        // Free space just in case
    c_virtual = move(c[current-1]);  
    c[current-1] = vector<int>();               // Free space just in case               
    w_virtual = move(w[current-1]);
    w[current-1] = vector<int>();
    if(track_solution)
    {
        p_virtual = vector<int>(c_virtual.size(),0);  //P
        iota(p_virtual.begin(), p_virtual.end(), 0);              //P, initalizes p to right position, itself
    }
}

template<typename T>
void Smartstorage<T>::end_virtual_path_no_files(const int current, const RootedTree& RT,
    vector<T>& valid_virtual,
    vector<T>& domination_virtual,
    vector<int>& c_virtual, 
    vector<int>& w_virtual,
    vector<int>& p_virtual,
    vector<int>& number_of_neighbours_forgotten_virtual,
    vector<int>& number_of_neighbours_present_virtual
)
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
        number_of_neighbours_forgotten[parent-1] = move(number_of_neighbours_forgotten_virtual);
        number_of_neighbours_present[parent-1] = move(number_of_neighbours_present_virtual);

        validcandidates[parent-1] = move(valid_virtual);
        domination[parent-1] = move(domination_virtual);
        c[parent-1] = move(c_virtual);
        w[parent-1] = move(w_virtual);
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
        // 
        T all_neighbours_accounted_for_mask = 0;

        for(size_t j = 0; j < bags[parent-1].size(); j++)
        {
            number_of_neighbours_forgotten[parent-1][j] += number_of_neighbours_forgotten_virtual[j];
            if(number_of_neighbours_forgotten[parent-1][j] + number_of_neighbours_present[parent-1][j] == G.N[bags[parent-1][j]-1].size())
                all_neighbours_accounted_for_mask |= (T(1) << j); 
        }

        // Start doing: if intersecting, keep if all all_neighbours_accounted_for_mask are also dominated
        vector<T> valid_temporary;
        vector<T> domination_temporary;
        vector<int> c_temporary;
        vector<int> w_temporary;
        vector<int> p_temporary;

        size_t index_virtual = 0;
        for(size_t index_parent = 0; index_parent < validcandidates[parent-1].size(); index_parent++)
        {
            while(index_virtual < valid_virtual.size() && valid_virtual[index_virtual] < validcandidates[parent-1][index_parent])
                index_virtual++;
            if(index_virtual < valid_virtual.size() && valid_virtual[index_virtual] == validcandidates[parent-1][index_parent])
            {
                // consider adding
                if(((domination[parent-1][index_parent] | domination_virtual[index_virtual]) & all_neighbours_accounted_for_mask) == all_neighbours_accounted_for_mask)
                {
                    valid_temporary.push_back(validcandidates[parent-1][index_parent]);
                    domination_temporary.push_back(domination[parent-1][index_parent] | domination_virtual[index_virtual]);
                    c_temporary.push_back(c[parent-1][index_parent] + c_virtual[index_virtual]);
                    w_temporary.push_back(w[parent-1][index_parent]);
                    
                    if(track_solution)
                    {
                        // p
                    }
                }
            }
        }
        validcandidates[parent-1] = move(valid_temporary);
        domination[parent-1] = move(domination_temporary);
        c[parent-1] = move(c_temporary);
        w[parent-1] = move(w_temporary);
        
        if(track_solution)
        {
            // P; P is an issue
        }

        /*
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
        */
    }
}


/*
template<typename T>
void Smartstorage<T>::take_virtual_step_introduce(
    const int current_virtual, 
    vector<vector<T>>& valid_virtual,
    vector<vector<T>>& domination_virtual,
    vector<vector<int>>& c_virtual, 
    vector<vector<int>>& w_virtual,
    vector<vector<int>>& p_virtual, 
    const int i,
    const vector<int>& bag_virtual,
    vector<int>& number_of_neighbours_forgotten_virtual,
    vector<int>& number_of_neighbours_present_virtual
    )
{
    int parent_virtual = (current_virtual%2) + 1;

    valid_virtual[parent_virtual-1].resize(0);
    domination_virtual[parent_virtual-1].resize(0);
    c_virtual[parent_virtual-1].resize(0);
    w_virtual[parent_virtual-1].resize(0);

    if(track_solution)
    {
        p_virtual[parent_virtual-1].resize(0);
    }

    number_of_neighbours_forgotten_virtual.insert(number_of_neighbours_forgotten_virtual.begin()+i, 0);
    number_of_neighbours_present_virtual.insert(number_of_neighbours_present_virtual.begin()+i, 0);

    T adjacency_mask = 0;
    T all_neighbours_accounted_mask = 0;

    int k = 0;
    for(int j = 0; j < bag_virtual.size(); j++)
    {
        while(k<G.N[bag_virtual[i]-1].size() && G.N[bag_virtual[i]-1][k] < bag_virtual[j])
            k++;
        if(k<G.N[bag_virtual[i]-1].size() && G.N[bag_virtual[i]-1][k] == bag_virtual[j])
        {
            adjacency_mask |= (T(1) << j);
            number_of_neighbours_present_virtual[i]++;
            number_of_neighbours_present_virtual[j]++;
        }
        // Moved it outside of above if statement
        if(number_of_neighbours_forgotten_virtual[j] + number_of_neighbours_present_virtual[j] ==  G.N[bag_virtual[j]-1].size())
            all_neighbours_accounted_mask |= (T(1) << j);
    }
    // number_of_neighbours_present[i] only fully updated after loop(doing it extra for i in loop is fine)
    if(number_of_neighbours_forgotten_virtual[i] + number_of_neighbours_present_virtual[i] ==  G.N[bag_virtual[i]-1].size())
        all_neighbours_accounted_mask |= (T(1) << i);

    size_t remember_start = 0;
    size_t remember_end = 0;

    T lowerbitmask = (T(1)<<i) - 1;

    for(size_t current_index = 0; current_index < valid_virtual[current_virtual-1].size();)
    {
        T lower = valid_virtual[current_virtual-1][current_index] & lowerbitmask;
        T higher = (valid_virtual[current_virtual-1][current_index] - lower) << 1;

        T lower_start = valid_virtual[current_virtual-1][remember_start] & lowerbitmask;
        T higher_start = (valid_virtual[current_virtual-1][remember_start] - lower_start) << 1;

        if(higher == higher_start)
        {
            T domination_lower = domination_virtual[current_virtual-1][current_index] & lowerbitmask;
            T domination_higher = (domination_virtual[current_virtual-1][current_index] - domination_lower) << 1;

            bool domination_status = (T((lower+higher) & adjacency_mask) != 0);
            if(((domination_higher + (T(domination_status)<< i) + domination_lower) & all_neighbours_accounted_mask) == all_neighbours_accounted_mask)
            {
                valid_virtual[parent_virtual-1].push_back(higher | lower);
                domination_virtual[parent_virtual-1].push_back(domination_higher + (T(domination_status)<< i) + domination_lower);
                c_virtual[parent_virtual-1].push_back(c_virtual[current_virtual-1][current_index]);
                w_virtual[parent_virtual-1].push_back(w_virtual[current_virtual-1][current_index]);
                if(track_solution)
                {
                    p_virtual[parent_virtual-1].push_back(p_virtual[current_virtual-1][current_index]);
                }
            }
            remember_end = current_index;
            current_index++;
        }
        else
        {
            for(;remember_start <= remember_end; remember_start++)
            {
                T lower_start = valid_virtual[current_virtual-1][remember_start] & lowerbitmask;
                T higher_start = (valid_virtual[current_virtual-1][remember_start] - lower_start) << 1;

                T domination_lower = domination_virtual[current_virtual-1][remember_start] & lowerbitmask;
                T domination_higher = (domination_virtual[current_virtual-1][remember_start] - domination_lower) << 1;

                if((adjacency_mask & (higher_start | lower_start)) == 0)  // Where is the domination check?
                {
                    valid_virtual[parent_virtual-1].push_back(higher_start | (T(1)<<i) | lower_start);
                    domination_virtual[parent_virtual-1].push_back((domination_higher + (T(1)<<i) + domination_lower) | adjacency_mask);
                    c_virtual[parent_virtual-1].push_back(c_virtual[current_virtual-1][remember_start] + G.weights[bag_virtual[i]-1]);
                    w_virtual[parent_virtual-1].push_back(w_virtual[current_virtual-1][remember_start] + G.weights[bag_virtual[i]-1]);
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

        T domination_lower = domination_virtual[current_virtual-1][remember_start] & lowerbitmask;
        T domination_higher = (domination_virtual[current_virtual-1][remember_start] - domination_lower) << 1;

        if((adjacency_mask & (higher_start | lower_start)) == 0)
        {
            valid_virtual[parent_virtual-1].push_back(higher_start | (T(1)<<i) | lower_start);
            domination_virtual[parent_virtual-1].push_back((domination_higher + (T(1)<<i) + domination_lower) | adjacency_mask);
            c_virtual[parent_virtual-1].push_back(c_virtual[current_virtual-1][remember_start]+G.weights[bag_virtual[i]-1]);
            w_virtual[parent_virtual-1].push_back(w_virtual[current_virtual-1][remember_start]+G.weights[bag_virtual[i]-1]);
            if(track_solution)
            {
                p_virtual[parent_virtual-1].push_back(p_virtual[current_virtual-1][remember_start]);
            }
        }
    }
}

*/
