#ifndef GRAPH_H_INCLUDED
#define GRAPH_H_INCLUDED

//INCLUDE STD
#include <vector>
#include <string>

class Graph
{
public:
    std::vector<std::vector<int>> N;          //N[vertex-1]: is the Neighborhood of vertex
    std::vector<int> weights;

    Graph(std::string ifname);
    void print() const;

    bool adjacent(int u, int v) const;

    void dfs(const int start) const;        //Doesn't exist
    void bfs(const int start) const;        //Doesn't exist

    bool independent_set(const std::vector<int>& subset) const;

    int weight_set(const std::vector<int>& subset) const;
};

#endif // GRAPH_H_INCLUDED
