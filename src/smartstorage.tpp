#include "smartstorage.h"

#include <iostream>
#include <algorithm>
#include <vector>
#include <numeric>
#include <cmath>
#include <stdint.h>
#include <stdio.h>
#include <cassert>

//INCLUDE OWN FILES
#include "graph.h"
#include "rootedtree.h"
#include "general.h"
#include "addressable_priority_queue.h"
#include "store_on_file.h"

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
    /*
    cout << "At node " << current << endl;
    cout << "Bag:" << endl;
    print_vector(bags[current-1]);
    cout << "Forgotten:" << endl;
    print_vector(number_of_neighbours_forgotten[current-1]);
    cout << "Present:" << endl;
    print_vector(number_of_neighbours_present[current-1]);
    cout << "Valid:" << endl;
    print_vector(validcandidates[current-1]);
    cout << "Domination:" << endl;
    print_vector(domination[current-1]);
    cout << "c:" << endl;
    print_vector(c[current-1]);
    cout << "w:" << endl;
    print_vector(w[current-1]);
    */
    //cout << log2(validcandidates[current-1].size()) << "," << bags[current-1].size() << endl;
    //cout << endl;
    // */
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
    //cout << max_element(c[current-1].begin(), c[current-1].end())-c[current-1].begin() << endl;
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

    //cout << "WALKING FROM " << current << " TO " << parent << ":" << endl;

    const vector<int>& source_bag = bags[current-1];
    const vector<int>& target_bag = bags[parent-1];

    int current_virtual = 1;
    int parent_virtual = 2;


    vector<vector<int>> c_virtual(2);
    vector<vector<int>> w_virtual(2);
    vector<vector<int>> p_virtual(2);       // P
    vector<vector<T>> valid_virtual(2);
    vector<vector<T>> domination_virtual(2);

    begin_virtual_path(current, RT, 
        valid_virtual[current_virtual-1],
        domination_virtual[current_virtual-1],
        c_virtual[current_virtual-1],
        w_virtual[current_virtual-1],
        p_virtual[current_virtual-1]);

    vector<int> bag_virtual = source_bag;
    vector<int> number_of_neighbours_forgotten_virtual = number_of_neighbours_forgotten[current-1];
    vector<int> number_of_neighbours_present_virtual = number_of_neighbours_present[current-1];


    vector<int> vertices_to_introduce;
    int forgotten_so_far = 0;
    int index_source = 0;
    for(int index_target = 0; index_target<target_bag.size(); index_target++)
    {
        while(index_source<source_bag.size() && source_bag[index_source]<target_bag[index_target])
        {
            int i = index_source-forgotten_so_far;
            bag_virtual.erase(bag_virtual.begin() + i);
            take_virtual_step_forget(current_virtual, 
                valid_virtual,
                domination_virtual,
                c_virtual, 
                w_virtual,
                p_virtual, 
                i, 
                source_bag[index_source],
                bag_virtual, 
                number_of_neighbours_forgotten_virtual, 
                number_of_neighbours_present_virtual);
            swap(current_virtual, parent_virtual);
            forgotten_so_far++;
            index_source++;
        }
        if(index_source<source_bag.size() && source_bag[index_source]==target_bag[index_target])
            index_source++;
        else
            vertices_to_introduce.push_back(target_bag[index_target]);

    }
    while(index_source<source_bag.size())
    {
        int i = index_source-forgotten_so_far;
        bag_virtual.erase(bag_virtual.begin()+i);
        take_virtual_step_forget(current_virtual, 
            valid_virtual,
            domination_virtual,
            c_virtual, 
            w_virtual,
            p_virtual, 
            i, 
            source_bag[index_source],
            bag_virtual, 
            number_of_neighbours_forgotten_virtual, 
            number_of_neighbours_present_virtual);
        swap(current_virtual, parent_virtual);
        forgotten_so_far++;
        index_source++;
    }

    //cout << log2(valid_virtual[current_virtual-1].size()) << "," << bag_virtual.size() << endl;

    AddressablePriorityQueue<int,greater<int>,plus<int>> APQ(vertices_to_introduce.size());
    for(int x = 1; x<=vertices_to_introduce.size(); x++)
    {
        int key = 0;
        for(int y = 0; y < bag_virtual.size(); y++)
            if(G.adjacent(vertices_to_introduce[x-1],bag_virtual[y]))
                key++;
        APQ.insertElement(x,key);
    }
    for(int temp = 0; temp<vertices_to_introduce.size();temp++)
    {
        int key = APQ.v[0].second;
        int x = APQ.deleteRoot();
        //if(key == 0)
        //{
            //cout << "Warning; trying to introduce " << vertices_to_introduce[x-1] << " with key 0 which has degree " << G.N[vertices_to_introduce[x-1]-1].size() << endl; 
        //    if(validcandidates[parent-1].size()>0)
        //    {
        //        int positioninparent = lower_bound(bags[parent-1].begin(),bags[parent-1].end(),vertices_to_introduce[x-1]) - bags[parent-1].begin();
        //        cout << "Looking for " 
        //            << G.N[vertices_to_introduce[x-1]-1].size() - number_of_neighbours_present[parent-1][positioninparent] - number_of_neighbours_forgotten[parent-1][positioninparent] 
        //            << " neighbours" << endl;
        //    }
        //        
        //}
        auto it = lower_bound(bag_virtual.begin(), bag_virtual.end(), vertices_to_introduce[x-1]);
            

        int i = it - bag_virtual.begin();// Relevant bitposition

        bag_virtual.insert(it, vertices_to_introduce[x-1]);

        take_virtual_step_introduce(current_virtual, 
                valid_virtual,
                domination_virtual,
                c_virtual,
                w_virtual,
                p_virtual, 
                i, 
                bag_virtual,
                number_of_neighbours_forgotten_virtual,
                number_of_neighbours_present_virtual);
        swap(current_virtual, parent_virtual);
        //cout << log2(valid_virtual[current_virtual-1].size()) << "," << bag_virtual.size() << endl;
        for(int y = 0; y<vertices_to_introduce.size(); y++)
            if(G.adjacent(vertices_to_introduce[x-1],vertices_to_introduce[y]) && APQ.p[y]!=-1)
                APQ.updateKey(y+1,1);
    }

    //cout << log2(valid_virtual[current_virtual-1].size()) << "," << bag_virtual.size() << endl;

    end_virtual_path(current, RT, 
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

    number_of_neighbours_forgotten_virtual.insert(number_of_neighbours_forgotten_virtual.begin() + i, 0);
    number_of_neighbours_present_virtual.insert(number_of_neighbours_present_virtual.begin() + i, 0);

    T adjacency_mask = 0;
    T all_neighbours_accounted_for_mask = 0;

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
            all_neighbours_accounted_for_mask |= (T(1) << j);
    }
    // number_of_neighbours_present[i] only fully updated after loop(doing it extra for i in loop is fine)
    if(number_of_neighbours_forgotten_virtual[i] + number_of_neighbours_present_virtual[i] ==  G.N[bag_virtual[i]-1].size())
        all_neighbours_accounted_for_mask |= (T(1) << i);

    size_t current_index_zero = 0;
    size_t current_index_one = 0;

    T lowerbitmask = (T(1)<<i) - 1;

    while(current_index_zero < valid_virtual[current_virtual-1].size() && current_index_one < valid_virtual[current_virtual-1].size())
    {
        T lower_zero = valid_virtual[current_virtual-1][current_index_zero] & lowerbitmask;
        T higher_zero = (valid_virtual[current_virtual-1][current_index_zero] - lower_zero) << 1;
        T lower_one = valid_virtual[current_virtual-1][current_index_one] & lowerbitmask;
        T higher_one = (valid_virtual[current_virtual-1][current_index_one] - lower_one) << 1;
        if((higher_zero | lower_zero) < (higher_one | (T(1) << i) | lower_one))
        {
            T lower_zero_domination = domination_virtual[current_virtual-1][current_index_zero] & lowerbitmask;
            T higher_zero_domination = ((domination_virtual[current_virtual-1][current_index_zero] - lower_zero_domination) << 1);

            T domination_status_zero = ((adjacency_mask & (higher_zero | lower_zero)) != 0);

            if(((higher_zero_domination | (T(domination_status_zero)<<i) | lower_zero_domination) & all_neighbours_accounted_for_mask) == all_neighbours_accounted_for_mask)
            {
                valid_virtual[parent_virtual-1].push_back(higher_zero | lower_zero);
                domination_virtual[parent_virtual-1].push_back(higher_zero_domination | (T(domination_status_zero)<<i) | lower_zero_domination);
                c_virtual[parent_virtual-1].push_back(c_virtual[current_virtual-1][current_index_zero]);
                w_virtual[parent_virtual-1].push_back(w_virtual[current_virtual-1][current_index_zero]);
                // P
            }
            current_index_zero++;
        }
        else
        {
            if((adjacency_mask & (higher_one | lower_one)) == 0)
            {
                T lower_one_domination = domination_virtual[current_virtual-1][current_index_one] & lowerbitmask;
                T higher_one_domination = ((domination_virtual[current_virtual-1][current_index_one] - lower_one_domination) << 1);
                valid_virtual[parent_virtual-1].push_back(higher_one | (T(1)<<i) | lower_one);
                domination_virtual[parent_virtual-1].push_back(higher_one_domination | (T(1)<<i) | lower_one_domination | adjacency_mask);
                c_virtual[parent_virtual-1].push_back(c_virtual[current_virtual-1][current_index_one]+G.weights[bag_virtual[i]-1]);
                w_virtual[parent_virtual-1].push_back(w_virtual[current_virtual-1][current_index_one]+G.weights[bag_virtual[i]-1]);
            }
            current_index_one++;
        }
    }
    // Do remainder
    while(current_index_zero < valid_virtual[current_virtual-1].size())
    {
        T lower_zero = valid_virtual[current_virtual-1][current_index_zero] & lowerbitmask;
        T higher_zero = (valid_virtual[current_virtual-1][current_index_zero] - lower_zero) << 1;

        T lower_zero_domination = domination_virtual[current_virtual-1][current_index_zero] & lowerbitmask;
        T higher_zero_domination = ((domination_virtual[current_virtual-1][current_index_zero] - lower_zero_domination) << 1);

        T domination_status = ((adjacency_mask & valid_virtual[current_virtual-1][current_index_zero]) != 0);

        if(((higher_zero_domination | (T(domination_status)<<i) |lower_zero_domination) & all_neighbours_accounted_for_mask) == all_neighbours_accounted_for_mask)
        {
            valid_virtual[parent_virtual-1].push_back(higher_zero | lower_zero);
            domination_virtual[parent_virtual-1].push_back(higher_zero_domination | (T(domination_status)<<i) | lower_zero_domination);
            c_virtual[parent_virtual-1].push_back(c_virtual[current_virtual-1][current_index_zero]);
            w_virtual[parent_virtual-1].push_back(w_virtual[current_virtual-1][current_index_zero]);
                // P
        }
        current_index_zero++;
    }
    while(current_index_one < valid_virtual[current_virtual-1].size())
    {
        T lower_one = valid_virtual[current_virtual-1][current_index_one] & lowerbitmask;
        T higher_one = ((valid_virtual[current_virtual-1][current_index_one] - lower_one) << 1);
        if((adjacency_mask & (higher_one | lower_one)) == 0)
        {
            T lower_one_domination = domination_virtual[current_virtual-1][current_index_one] & lowerbitmask;
            T higher_one_domination = ((domination_virtual[current_virtual-1][current_index_one] - lower_one_domination) << 1);
            valid_virtual[parent_virtual-1].push_back(higher_one | (T(1)<<i) | lower_one);
            domination_virtual[parent_virtual-1].push_back(higher_one_domination | (T(1)<<i) | lower_one_domination | adjacency_mask);
            c_virtual[parent_virtual-1].push_back(c_virtual[current_virtual-1][current_index_one]+G.weights[bag_virtual[i]-1]);
            w_virtual[parent_virtual-1].push_back(w_virtual[current_virtual-1][current_index_one]+G.weights[bag_virtual[i]-1]);
        }
        current_index_one++;
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

    number_of_neighbours_forgotten_virtual.erase(number_of_neighbours_forgotten_virtual.begin() + i);
    number_of_neighbours_present_virtual.erase(number_of_neighbours_present_virtual.begin() + i);

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
        if((higher_zero | lower_zero) <= (higher_one | lower_one))
        {
            valid_virtual[parent_virtual-1].push_back(higher_zero | lower_zero);
            if(track_solution)
            {
                p_virtual[parent_virtual-1].push_back(p_virtual[current_virtual-1][current_index_zero]);
            }
            T lower_zero_domination = domination_virtual[current_virtual-1][current_index_zero] & lowerbitmask;
            bool lower_domination_status = ((domination_virtual[current_virtual-1][current_index_zero] & (T(1) << i)) != 0); 
            T higher_zero_domination = (domination_virtual[current_virtual-1][current_index_zero] - lower_zero_domination - (T(lower_domination_status)<<i)) >> 1;
            domination_virtual[parent_virtual-1].push_back(higher_zero_domination | lower_zero_domination);
            c_virtual[parent_virtual-1].push_back(c_virtual[current_virtual-1][current_index_zero]);
            w_virtual[parent_virtual-1].push_back(w_virtual[current_virtual-1][current_index_zero]);
            if((higher_zero | lower_zero) == (higher_one | lower_one))
            {
                if(c_virtual[current_virtual-1][current_index_one] > c_virtual[current_virtual-1][current_index_zero])
                {
                    T lower_one_domination = domination_virtual[current_virtual-1][current_index_one] & lowerbitmask;
                    T higher_one_domination = (domination_virtual[current_virtual-1][current_index_one] - lower_one_domination - (T(1)<<i)) >> 1;
                    valid_virtual[parent_virtual-1].back() = (higher_one | lower_one);
                    if(track_solution)
                    {
                        p_virtual[parent_virtual-1].back() = p_virtual[current_virtual-1][current_index_one];
                    }
                    domination_virtual[parent_virtual-1].back() = (higher_one_domination | lower_one_domination);
                    c_virtual[parent_virtual-1].back() = c_virtual[current_virtual-1][current_index_one];
                    // w. Needn't update w
                    //assert(w_virtual[current_virtual-1][current_index_zero] == w_virtual[current_virtual-1][current_index_one] - G.weights[forgotten_vertex-1]);
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
            valid_virtual[parent_virtual-1].push_back(higher_one | lower_one);
            if(track_solution)
            {
                p_virtual[parent_virtual-1].push_back(p_virtual[current_virtual-1][current_index_one]);
            }
            T lower_one_domination = domination_virtual[current_virtual-1][current_index_one] & lowerbitmask;
            T higher_one_domination = (domination_virtual[current_virtual-1][current_index_one] - lower_one_domination - (T(1)<<i)) >> 1;
            domination_virtual[parent_virtual-1].push_back(higher_one_domination | lower_one_domination);
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
        valid_virtual[parent_virtual-1].push_back(higher_zero | lower_zero);
        if(track_solution)
        {
            p_virtual[parent_virtual-1].push_back(p_virtual[current_virtual-1][current_index_zero]);
        }
        T lower_zero_domination = domination_virtual[current_virtual-1][current_index_zero] & lowerbitmask;
        bool lower_domination_status = ((domination_virtual[current_virtual-1][current_index_zero] & (T(1) << i)) != 0); 
        T higher_zero_domination = (domination_virtual[current_virtual-1][current_index_zero] - lower_zero_domination - (T(lower_domination_status)<<i)) >> 1;
        domination_virtual[parent_virtual-1].push_back(higher_zero_domination | lower_zero_domination);
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
        valid_virtual[parent_virtual-1].push_back(higher_one | lower_one);
        if(track_solution)
        {
            p_virtual[parent_virtual-1].push_back(p_virtual[current_virtual-1][current_index_one]);
        }
        T lower_one_domination = domination_virtual[current_virtual-1][current_index_one] & lowerbitmask;
        T higher_one_domination = (domination_virtual[current_virtual-1][current_index_one] - lower_one_domination - (T(1)<<i)) >> 1;
        domination_virtual[parent_virtual-1].push_back(higher_one_domination | lower_one_domination);
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

    //cout << "LEAF " << current << ":" << endl;

    const vector<int>& source_bag = vector<int>();
    const vector<int>& target_bag = bags[parent-1];

    int current_virtual = 1;
    int parent_virtual = 2;

    vector<vector<T>> valid_virtual(2);
    vector<vector<T>> domination_virtual(2);

    vector<vector<int>> c_virtual(2);
    vector<vector<int>> w_virtual(2);
    vector<vector<int>> p_virtual(2);       //P

    {////////////////////////////    begin_virtual_path Leaf version
        valid_virtual[current_virtual-1] = vector<T>(1,T(0));
        domination_virtual[current_virtual-1] = vector<T>(1,T(0));
        c_virtual[current_virtual-1] = vector<int>(1,0);
        w_virtual[current_virtual-1] = vector<int>(1,0);
        //if(track_solution)
        //{
        //    p_virtual[current_virtual-1] = vector<int>(1,0);  //P
        //    iota(p_virtual[current_virtual-1].begin(), p_virtual[current_virtual-1].end(), 0);              //P, initalized p to correct position, itself
        //}
    }/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    vector<int> bag_virtual = source_bag;
    vector<int> number_of_neighbours_forgotten_virtual = vector<int>();
    vector<int> number_of_neighbours_present_virtual = vector<int>();

    const vector<int>& vertices_to_introduce = target_bag; 

    AddressablePriorityQueue<int,greater<int>,plus<int>> APQ(vertices_to_introduce.size());
    for(int x = 1; x<=vertices_to_introduce.size(); x++)
    {
        int key = 0;
        for(int y = 0; y < bag_virtual.size(); y++)
            if(G.adjacent(vertices_to_introduce[x-1],bag_virtual[y]))
                key++;
        APQ.insertElement(x,key);
    }
    for(int temp = 0; temp<vertices_to_introduce.size();temp++)
    {
        int key = APQ.v[0].second;
        int x = APQ.deleteRoot();
        //if(key == 0)
        //    cout << "Warning; trying to introduce " << x << " with key 0." << endl;

        auto it = lower_bound(bag_virtual.begin(), bag_virtual.end(), vertices_to_introduce[x-1]);
        int i = it - bag_virtual.begin();// Relevant bitposition
        bag_virtual.insert(it, vertices_to_introduce[x-1]);
        take_virtual_step_introduce(current_virtual, 
                valid_virtual,
                domination_virtual,
                c_virtual,
                w_virtual,
                p_virtual, 
                i, 
                bag_virtual,
                number_of_neighbours_forgotten_virtual,
                number_of_neighbours_present_virtual);
        swap(current_virtual, parent_virtual);
        //cout << log2(valid_virtual[current_virtual-1].size()) << "," << bag_virtual.size() << endl;
        for(int y = 0; y<vertices_to_introduce.size(); y++)
            if(G.adjacent(vertices_to_introduce[x-1],vertices_to_introduce[y]) && APQ.p[y]!=-1)
                APQ.updateKey(y+1,1);
    }

    {//END VIRTUAL PATH LEAF VERSION
        validcandidates[parent-1] = move(valid_virtual[current_virtual-1]);
        domination[parent-1] = move(domination_virtual[current_virtual-1]);
        c[parent-1] = move(c_virtual[current_virtual-1]);
        w[parent-1] = move(w_virtual[current_virtual-1]);
        //if(track_solution)
        //{
        //        p[parent-1][neighbourposition] = move(p_virtual[current_virtual-1]);
        //}
        number_of_neighbours_forgotten[parent-1] = move(number_of_neighbours_forgotten_virtual);
        number_of_neighbours_present[parent-1] = move(number_of_neighbours_present_virtual);
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
    w[current-1] = vector<int>();               // Free space just in case 
    if(track_solution)
    {
        p_virtual = vector<int>(c_virtual.size(),0);  //P
        iota(p_virtual.begin(), p_virtual.end(), 0);              //P, initalizes p to correct position, itself
    }
}

template<typename T>
void Smartstorage<T>::end_virtual_path(const int current, const RootedTree& RT,
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
        validcandidates[parent-1] = move(valid_virtual);
        domination[parent-1] = move(domination_virtual);
        c[parent-1] = move(c_virtual);
        w[parent-1] = move(w_virtual);
        //if(track_solution)
        //{
        //    p[parent-1][neighbourposition] = move(p_virtual);
        //    compress_p_to_file(parent,neighbourposition,p[parent-1][neighbourposition]);
        //    p[parent-1][neighbourposition] = vector<int>();
        //}
        number_of_neighbours_forgotten[parent-1] = move(number_of_neighbours_forgotten_virtual);
        number_of_neighbours_present[parent-1] = move(number_of_neighbours_present_virtual);
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

        if(store_c)
        {
            validcandidates[parent-1] = decompress_from_file<T>("v_"+to_string(parent)+".zstd");
            domination[parent-1] = decompress_from_file<T>("d_"+to_string(parent)+".zstd");
            c[parent-1] = decompress_from_file<int>("c_"+to_string(parent)+".zstd");
            w[parent-1] = decompress_from_file<int>("w_"+to_string(parent)+".zstd");
            // Delete temp files now that data is loaded
            std::remove(("v_"+to_string(parent)+".zstd").c_str());
            std::remove(("d_"+to_string(parent)+".zstd").c_str());
            std::remove(("c_"+to_string(parent)+".zstd").c_str());
            std::remove(("w_"+to_string(parent)+".zstd").c_str());
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
                    //if(track_solution)
                    //{
                    //    
                    //}
                }
            }
        }
        validcandidates[parent-1] = move(valid_temporary);
        domination[parent-1] = move(domination_temporary);
        c[parent-1] = move(c_temporary);
        w[parent-1] = move(w_temporary);
        //if(track_solution)
        //{
        //    p[parent-1][neighbourposition] = move(p_virtual);
        //    compress_p_to_file(parent,neighbourposition,p[parent-1][neighbourposition]);
        //    p[parent-1][neighbourposition] = vector<int>();
        //}
    }
    if(store_c)
    {
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
            compress_to_file<T>(validcandidates[parent-1], "v_"+to_string(parent)+".zstd");
            validcandidates[parent-1] = vector<T>();
            compress_to_file<T>(domination[parent-1], "d_"+to_string(parent)+".zstd");
            domination[parent-1] = vector<T>();
            compress_to_file<int>(c[parent-1], "c_"+to_string(parent)+".zstd");
            c[parent-1] = vector<int>();
            compress_to_file<int>(w[parent-1], "w_"+to_string(parent)+".zstd");
            w[parent-1] = vector<int>();
        }
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
