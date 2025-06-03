#include "storage.h"

#include <iostream>
#include <algorithm>
#include <vector>
#include <numeric>
#include <set>

//INCLUDE OWN FILES
#include "graph.h"
#include "rootedtree.h"

using namespace std;

void Storage::density_checker() const
{
    for(int b = 0; b<bags.size(); b++)
    {
        int edges_in_bag = 0;
        vector<vector<int>> adjacency_matrix(bags[b].size(),vector<int>(bags[b].size(),0));
        for(int i = 0; i< bags[b].size(); i++)
        {
            for(int j = 0; j< bags[b].size(); j++)
            {
                if(G.adjacent(bags[b][i],bags[b][j]))
                {
                    adjacency_matrix[i][j] = 1;
                    edges_in_bag++;
                }
            }
        }
        edges_in_bag/=2;
        if(bags[b].size()>25)
            cout << b+1 << ": " << bags[b].size() << " vertices " << edges_in_bag << " edges " << 
            static_cast<double>(edges_in_bag)/static_cast<double>(bags[b].size()) << endl;
    }
}

void Storage::isolated_vertices_checker() const
{
    for(int b = 0; b<bags.size(); b++)
    {
        int isolated_vertices_count = 0;
        for(int i = 0; i<bags[b].size();i++)
        {
            bool is_isolated = true;
            for(int j = 0; j<bags[b].size(); j++)
                if(G.adjacent(bags[b][i],bags[b][j]))
                    is_isolated = false;
            if(is_isolated==true)
                isolated_vertices_count++;
        }
        if(bags[b].size()>25)
            cout << b+1 << " has " << isolated_vertices_count << "/" << bags[b].size() << "isolated vertices" << endl;
    }
}

//Order of initialization list relevant, therefore odd naming
Storage::Storage(const Graph& G, const vector<vector<int>>& bags):G(G), bags(bags)
{

}

void Storage::setup(const RootedTree& RT)
{
    int number_of_nodes = RT.N.size();                           //MOVE TO CONSTRUCTOR?
    c.resize(number_of_nodes);                                  //setup
    p.resize(number_of_nodes);                                  //setup
    w.resize(number_of_nodes);                                  //setup
}

void Storage::discover(const int& current, const RootedTree& RT)    //Prepare current node
{

    int size_DP_table = (1 << bags[current-1].size());                  ///////////////////////////////////
    c[current-1] = vector<int>( size_DP_table,0); //consider assign     ///////////////////////////////////
    p[current-1].resize(RT.N[current-1].size());

    for(int j = 0; j<RT.N[current-1].size();j++)
    {
        if(j!=RT.neighbourIterators[current-1] - RT.N[current-1].begin())
            p[current-1][j].resize(size_DP_table);// = vector<int>(size_DP_table,0);            //Skip the parent, seems to save time
    }
    if(RT.N[current-1].size()>2 || current == RT.root)
    {
        w[current-1].resize(1 << bags[current-1].size());
        w[current-1][0] = 0;

        int msb_position = 0;
        int msb_value = 1;

        for(unsigned int candidate_index = 1; candidate_index < w[current-1].size(); candidate_index++)
        {
            if(candidate_index >= (msb_value << 1))
            {
                msb_value <<= 1;
                msb_position++;
            }
            if(candidate_index%2!=0)
                w[current-1][candidate_index] = w[current-1][candidate_index-1] + G.weights[bags[current-1][0]-1];
            else
                w[current-1][candidate_index] = w[current-1][candidate_index-msb_value] + G.weights[bags[current-1][msb_position]-1];
        }
    }
}

void Storage::finish(const int& current, const RootedTree& RT)      //Heavy
{
    if((RT.N[current-1].size() == 1 && current != RT.root) || (current == RT.root && RT.N[current-1].size() == 0))
        initialize_leaf(current);

    update_current(current, RT);

    if(current != RT.root)
        update_to_parent(current, RT);
}

void Storage::cleanup(const RootedTree& RT)                                    //EMPTY, except for print
{
    int current = RT.root;
    cout << *max_element(c[current-1].begin(), c[current-1].end()) << endl;
}

void Storage::update_to_parent(const int& current, const RootedTree& RT)
{
    walk_virtual_path(current,RT);
}

void Storage::walk_virtual_path(const int& current, const RootedTree& RT)                            //Chosen method of updating current information to parent information
{
    int parent = RT.parents[current-1];
    int neighbourposition = RT.neighbourIterators[parent-1] - RT.N[parent-1].begin();


    const vector<int>& source_bag = bags[current-1];
    const vector<int>& target_bag = bags[parent-1];

    int current_virtual = 1;
    int parent_virtual = 2;


    vector<vector<int>> c_virtual(2);
    vector<vector<int>> p_virtual(2);


    c_virtual[current_virtual-1] = c[current-1];
    p_virtual[current_virtual-1] = vector<int>(c[current-1].size(),0);
    iota(p_virtual[current_virtual-1].begin(), p_virtual[current_virtual-1].end(), 0);              //Init point to self

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

                    take_virtual_step_forget(current_virtual, c_virtual, p_virtual, i);

                    swap(current_virtual, parent_virtual);
                }//END FORGET BLOCK
                forgotten_so_far++;
                it_source_bag++;
            }
        }
    }
    while(it_source_bag != source_bag.end()) //forget whats left
    {
        //Forget *it_source_bag
        {//BEGIN FORGET BLOCK
            auto it = std::find(bag_virtual.begin(), bag_virtual.end(), *it_source_bag);
            bag_virtual.erase(it);

            int i = it_source_bag-source_bag.begin()-forgotten_so_far;

            take_virtual_step_forget(current_virtual, c_virtual, p_virtual, i);                                 //Replacement function, prob will need vertex represented for other applications
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
                    auto it = std::lower_bound(bag_virtual.begin(), bag_virtual.end(), *it_target_bag);
                    bag_virtual.insert(it, *it_target_bag);

                    int i = it_intersection_bag-intersection_bag.begin()+introduced_so_far;

                    take_virtual_step_introduce(current_virtual, c_virtual, p_virtual, i, bag_virtual);
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

            take_virtual_step_introduce(current_virtual, c_virtual, p_virtual, i, bag_virtual);
            swap(current_virtual, parent_virtual);

        }//END INTRODUCE BLOCK
        introduced_so_far++;
        it_target_bag++;
    }

    for(int index = 0; index < c[parent-1].size(); index++)
    {
        if(c[parent-1][index]!=-1)
        {
            if(c_virtual[current_virtual-1][index]==-1)
            {
                c[parent-1][index] = -1;
            }
            else
                c[parent-1][index] += c_virtual[current_virtual-1][index];
        }
    }
    p[parent-1][neighbourposition] = p_virtual[current_virtual-1];
}

void Storage::take_virtual_step_forget(const int& current_virtual, vector<vector<int>>& c_virtual, vector<vector<int>>& p_virtual, const int& i)
{
    int parent_virtual = (current_virtual%2) + 1;

    c_virtual[parent_virtual-1].resize(c_virtual[current_virtual-1].size()>>1); // = vector<int>(c_virtual[current_virtual-1].size()>>1);
    p_virtual[parent_virtual-1].resize(p_virtual[current_virtual-1].size()>>1); // = vector<int>(p_virtual[current_virtual-1].size()>>1);
    for(unsigned int index_parent = 0; index_parent < c_virtual[parent_virtual-1].size(); index_parent++)
    {
        int lower = index_parent & ((1 << i) - 1);
        int position_temporary = ((index_parent - lower) << 1) + lower;
        int index_current;
        if(c_virtual[current_virtual-1][position_temporary] >=  c_virtual[current_virtual-1][position_temporary + (1<<i)])
            index_current = position_temporary;
        else
            index_current = position_temporary + (1 << i);
        c_virtual[parent_virtual-1][index_parent] = c_virtual[current_virtual-1][index_current];
        p_virtual[parent_virtual-1][index_parent] = p_virtual[current_virtual-1][index_current];
    }
}

void Storage::take_virtual_step_introduce(const int& current_virtual, vector<vector<int>>& c_virtual, vector<vector<int>>& p_virtual, const int& i, const vector<int>& bag_virtual)
{
    int parent_virtual = (current_virtual%2) + 1;

    c_virtual[parent_virtual-1].resize(c_virtual[current_virtual-1].size()<<1);// = vector<int>(c_virtual[current_virtual-1].size()<<1);
    p_virtual[parent_virtual-1].resize(p_virtual[current_virtual-1].size()<<1);// = vector<int>(p_virtual[current_virtual-1].size()<<1);
    for(unsigned int index_parent = 0; index_parent <c_virtual[parent_virtual-1].size(); index_parent++)
    {
        //introduce it-h bag element
        int lower = index_parent & ((1<<i)-1);
        if(index_parent & (1<<i))                     //If ith bit is being set to 1
        {
            int index_current = ((index_parent - lower - (1<<i)) >> 1) + lower;
            c_virtual[parent_virtual-1][index_parent] = c_virtual[current_virtual-1][index_current] + G.weights[bag_virtual[i]-1];// + G.weights[*it_target_bag-1];
            p_virtual[parent_virtual-1][index_parent] = p_virtual[current_virtual-1][index_current];
            {//BEGIN INDEPENDENCY VIOLATED TEST
                if(index_parent != (1<<i))
                {   //Then there either exists a set bit of index_parent below i or above i, and let's call it j
                    int j;
                    if(lower>0)
                        j = countr_zero(index_parent);
                    else
                        j = 31 - countl_zero(index_parent);                     //31-0 is bit position range of an unsigned integer
                    //the vertex belonging to j is then bag_virtual[j]

                    //if index_parent - (1 << j) was an independent set then so is index_parent if bag_virual[i] not adjacent to bag_virtual[j]
                    if(c_virtual[parent_virtual-1][index_parent-(1<<j)] == -1 || G.adjacent(bag_virtual[i],bag_virtual[j]))
                    {
                        c_virtual[parent_virtual-1][index_parent] = -1;
                        //Overwrite c(not p)
                    }
                }
            }//END INDEPENDENCY VIOLATED TEST
        }
        else                                    //ith bit is being set to 0
        {
            int index_current = ((index_parent-lower)>>1) + lower;
            c_virtual[parent_virtual-1][index_parent] = c_virtual[current_virtual-1][index_current];
            p_virtual[parent_virtual-1][index_parent] = p_virtual[current_virtual-1][index_current];
        }
    }
}

void Storage::initialize_leaf(const int& current)
{
    c[current-1].resize(1 << bags[current-1].size());
    c[current-1][0] = 0;

    for(int i = 0; i<bags[current-1].size(); i++)
    {
        c[current-1][(1<<i)] = G.weights[bags[current-1][i]-1];
        for(unsigned int candidate = 1; candidate < (1 << i); candidate++)
        {
            int j = countr_zero(candidate);
            if(c[current-1][candidate + (1<<i) - (1<<j)] ==-1 || G.adjacent(bags[current-1][i],bags[current-1][j]))
                c[current-1][candidate + (1 << i)] = -1;
            else
                c[current-1][candidate + (1<<i)] = c[current-1][candidate] + G.weights[bags[current-1][i]-1];
        }
    }
} 

/*
void Storage::initialize_leaf(const int& current)                                       //Should be the same as walk but walking from a non-existing empty bag, could skip forget blocks
{
    //int neighbourposition = 0;
    vector<int> bag_empty(0);
    vector<int> c_empty(1,0);
    vector<int> p_empty(1,0);

    const vector<int>& source_bag = bag_empty;
    const vector<int>& target_bag = bags[current-1];

    int current_virtual = 1;
    int parent_virtual = 2;


    vector<vector<int>> c_virtual(2);
    vector<vector<int>> p_virtual(2);

    c_virtual[current_virtual-1] = c_empty;
    p_virtual[current_virtual-1] = p_empty;

    vector<int> bag_virtual = source_bag;

    vector<int> intersection_bag;
    set_intersection(source_bag.begin(), source_bag.end(),target_bag.begin(), target_bag.end(),back_inserter(intersection_bag));

    vector<int>::const_iterator it_source_bag = source_bag.begin();
    vector<int>::const_iterator it_intersection_bag = intersection_bag.begin();


    it_intersection_bag = intersection_bag.begin();
    vector<int>::const_iterator it_target_bag = target_bag.begin();

    //Introduce all vertices
    int introduced_so_far = 0;
    while(it_target_bag != target_bag.end())
    {
        //Introduce *it_target_bag
        {//Begin Introduce Block
            auto it = lower_bound(bag_virtual.begin(), bag_virtual.end(), *it_target_bag);
            bag_virtual.insert(it, *it_target_bag);

            int i = it_intersection_bag-intersection_bag.begin()+introduced_so_far;     //Relevant bitposition

            take_virtual_step_introduce(current_virtual, c_virtual, p_virtual, i, bag_virtual);                ///////////////////////

            swap(current_virtual, parent_virtual);

        }//END INTRODUCE BLOCK
        introduced_so_far++;
        it_target_bag++;
    }
    //UPDATE TO actual current                                                                                  ///////////////////
    for(int index = 0; index < c[current-1].size(); index++)
    {
        if(c[current-1][index]!=-1)
        {
            if(c_virtual[current_virtual-1][index]==-1)
            {
                c[current-1][index] = -1;
            }
            else
                c[current-1][index] += c_virtual[current_virtual-1][index];
        }
    }
    //p[current-1][neighbourposition] = p_virtual[current_virtual-1];
}
*/

void Storage::update_current(const int& current, const RootedTree& RT)      //Account for multiplicity
{
    if(RT.N[current-1].size()>2 && current != RT.root)
        for(unsigned int candidate_index = 0; candidate_index < c[current-1].size(); candidate_index++)
            if(c[current-1][candidate_index]!=-1)
                c[current-1][candidate_index] -= w[current-1][candidate_index]*(RT.N[current-1].size()-2);
    if(current == RT.root && RT.N[current-1].size()>1)
        for(unsigned int candidate_index = 0; candidate_index < c[current-1].size(); candidate_index++)
            if(c[current-1][candidate_index]!=-1)
                c[current-1][candidate_index] -= w[current-1][candidate_index]*(RT.N[current-1].size()-1);
}


void Storage::intersection_with_parent_bag(const int& current,const RootedTree& RT) const
{
    int parent = RT.parents[current-1];
    vector<int> result;
    set_intersection(
    bags[current-1].begin(), bags[current-1].end(),
        bags[parent-1].begin(), bags[parent-1].end(),
        back_inserter(result)
    );
    //cout << "Intersection between bag " << current << "and bag " << parent << ": " << result.size() << endl;

    int edges_in_intersection = 0;
    vector<vector<int>> adjacency_matrix(result.size(),vector<int>(result.size(),0));
        for(int i = 0; i< result.size(); i++)
        {
            for(int j = 0; j< result.size(); j++)
            {
                if(G.adjacent(result[i],result[j]))
                {
                    adjacency_matrix[i][j] = 1;
                    edges_in_intersection++;
                }
            }
        }
        edges_in_intersection/=2;
        cout << "Intersection between bag " << current << " and bag " << parent << ": " << result.size() << " vertices " << edges_in_intersection << " edges " << 
        static_cast<double>(edges_in_intersection)/static_cast<double>(result.size()) << endl;
}
