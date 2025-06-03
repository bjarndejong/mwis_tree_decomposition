//INCLUDE STD
#include <algorithm>
#include <iostream>
#include <vector>

//INCLUDE OWN FILES
#include "rootedtree.h"
#include "smartstorage.h"
#include "general.h"
#include "solution.h"

using namespace std;

template<typename T>
vector<int> bitmask_filter(const T& bitmask, const vector<int>& v)
{
    vector<int> filtered_vector;
    for(int i = 0; i<v.size(); i++)
        if(bitmask & (T(1)<<i))
            filtered_vector.push_back(v[i]);
    return filtered_vector;
}

template<typename T>
Solution<T>::Solution(const Smartstorage<T>& SMART, const int& positionMWIS):S(SMART), positionMWIS(positionMWIS)
{
}

template<typename T>
void Solution<T>::setup(const RootedTree& RT)
{
    //int current = RT.root;
    choices.resize(S.bags.size());
    SIZES.resize(S.bags.size());
}

template<typename T>
void Solution<T>::discover(const int& current, const RootedTree& RT)
{
    int parent = RT.parents[current-1];
    int neighbourposition = RT.neighbourIterators[parent-1] - RT.N[parent-1].begin();

    //Set up adjacency matrix for current bag
    vector<vector<int>> adjacency_matrix(S.bags[current-1].size(),vector<int>(S.bags[current-1].size(),0));
    for(int i = 0; i< S.bags[current-1].size(); i++)
    {
        for(int j = 0; j<S.bags[current-1].size(); j++)
        {
            if(S.G.adjacent(S.bags[current-1][i],S.bags[current-1][j]))
                adjacency_matrix[i][j] = 1;
        }
    }

    //Set up validcandidates for current bag
    vector<T> validcandidates(1,0);
    for(int i = 0; i<S.bags[current-1].size(); i++)
    {
        int vSize = validcandidates.size();
        validcandidates.push_back(T(1)<<i);
        for(int index = 1; index<vSize; index++)
        {
            //add validcandidates[index]+(T(1)<<i) if it is an independent set
            int j = countr_zero(validcandidates[index]);

            if(
                binary_search(validcandidates.begin(),    //CONSIDER DIFFERENT START AND END
                    validcandidates.end(),
                    validcandidates[index] + (T(1)<<i) - (T(1)<<j)) == true
                    &&
                    adjacency_matrix[i][j]==0
            )
            {
                validcandidates.push_back(validcandidates[index]+(T(1)<<i));
                //cout << current << ": " << validcandidates.size() << endl;
            }
        }
    }
    SIZES[current-1] = validcandidates.size();

    if(current == RT.root)
    {
        choices[current-1] = positionMWIS;//find(validcandidates.begin(),validcandidates.end(),valueMWIS) - validcandidates.begin();   //WRONG
        //max_element(S.c[current-1].begin(), S.c[current-1].end()) - S.c[current-1].begin();
    }
    else
    {
        vector<int> p;
        p.resize(SIZES[parent-1]);
        decompress_p_from_file(parent,neighbourposition,p);
        remove_p_file(parent,neighbourposition);
        choices[current-1] = p[choices[parent-1]]; //S.p[parent-1][neighbourposition][choices[parent-1]];
    }
    vector<int> v = bitmask_filter(validcandidates[choices[current-1]],S.bags[current-1]);

    vector<int> temp;

    set_union(MWIS.begin(),MWIS.end(),v.begin(),v.end(),back_inserter(temp));

    MWIS = temp;
}

template<typename T>                                            //EMPTY
void Solution<T>::finish(const int& current, const RootedTree& RT)
{
    
}

template<typename T>                                            //EMPTY(commented out print temporarily)
void Solution<T>::cleanup(const RootedTree& RT)
{
    //print_vector(MWIS);
}

