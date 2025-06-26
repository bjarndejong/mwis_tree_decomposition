#include "graph.h"

//INCLUDE STD
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

using namespace std;

Graph::Graph(vector<vector<int>>&& N, vector<int>&& weights)
    :N(move(N)), weights(move(weights))
{

}

Graph Graph::from_file(const std::string& ifname)                   // Static factory method
{
    ifstream ifs{ifname};
    if (!ifs)
    {
        cerr << "Error: Failed to open .graph file '" << ifname << "'\n";
        exit(1); // or return, depending on your function
    }
    string input_holder{};
    //Process first line:               input line: n m 10
    getline(ifs,input_holder);
    input_holder = input_holder.substr(0,input_holder.find_last_of(' '));
    input_holder = input_holder.substr(0,input_holder.find_last_of(' '));

    int num_of_vertices = stoi(input_holder);

    //Initialize
    vector<vector<int>> N(num_of_vertices,vector<int>());
    vector<int> weights(num_of_vertices,0);

    //Process remaining lines:               input line: w(v) N(v)
    int weight;
    int neighbour;

    int index = 0;
    while(index<num_of_vertices)
    {
        getline(ifs,input_holder);
        stringstream ss{input_holder};
        ss >> weight;
        weights[index] = weight;
        while(ss >> neighbour)
        {
            N[index].push_back(neighbour);
        }
        index++;
    }
    ifs.close();
    return Graph(move(N),move(weights));
}


void Graph::print() const
{
    cout << "Graph:" << endl;
    for(vector<vector<int>>::const_iterator vertex_it = N.begin(); vertex_it != N.end(); vertex_it++)
    {
        cout << "N(" << vertex_it-N.begin()+1 << ") = {";
        for(vector<int>::const_iterator adj_it = vertex_it->begin(); adj_it != vertex_it->end(); adj_it++)
        {
            cout << *adj_it << " ";
        }
        cout << "}" << endl;
    }
    for(vector<int>::const_iterator vertex_it = weights.begin(); vertex_it != weights.end(); vertex_it++)
    {
        cout << "w(" << vertex_it-weights.begin()+1 << ") = " << *vertex_it << endl;
    }
    cout << endl;
}


bool Graph::independent_set(const std::vector<int>& subset) const
{
    for(std::vector<int>::const_iterator it1 = subset.begin(); it1!=subset.end(); it1++)
        for(std::vector<int>::const_iterator it2 = it1+1; it2 != subset.end(); it2++)
            if(adjacent(*it1,*it2))
                return false;
    return true;
}

bool Graph::adjacent(int u, int v) const
{
    return binary_search(N[u-1].begin(),N[u-1].end(),v);
}

int Graph::weight_set(const std::vector<int>& subset) const
{
    int accumulated_weight = 0;
    for(std::vector<int>::const_iterator it = subset.begin(); it != subset.end(); it++)
        accumulated_weight += weights[(*it) - 1];
    return accumulated_weight;
}

