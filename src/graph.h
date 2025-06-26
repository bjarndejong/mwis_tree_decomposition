#ifndef GRAPH_H_INCLUDED
#define GRAPH_H_INCLUDED

//INCLUDE STD
#include <vector>
#include <string>

class Graph
{
public:
    const std::vector<std::vector<int>> N;                  //N[vertex-1]: is the Neighborhood of vertex
    const std::vector<int> weights;

    static Graph from_file(const std::string& ifname);      // Static factory method
 
    void print() const;

    bool adjacent(int u, int v) const;
    bool independent_set(const std::vector<int>& subset) const;
    int weight_set(const std::vector<int>& subset) const;
private:
    Graph(std::vector<std::vector<int>>&& N, std::vector<int>&& weights);
};

#endif // GRAPH_H_INCLUDED
