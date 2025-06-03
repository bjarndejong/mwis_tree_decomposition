#include "treedecomp.h"

//INCLUDE STD
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

//INCLUDE OWN HEADERS
#include "graph.h"

using namespace std;

void TreeDecomp::print_TreeDecomp() const
{
    cout << "Tree-Decomposition:" << endl;
    //Print neighbourhood of each node
    for(vector<vector<int>>::const_iterator node_it = N.begin(); node_it != N.end(); node_it++)
    {
        cout << "N(X_" << (node_it - N.begin())+1 << "): ";
        for(vector<int>::const_iterator adj_it = node_it->begin(); adj_it != node_it->end(); adj_it++)
        {
            cout << "X_" << *adj_it << ',' << ' ';
        }
        cout << endl;
    }
    //Print bag contents of each node
    for(vector<vector<int>>::const_iterator node_it = bags.begin(); node_it != bags.end(); node_it++)
    {
        cout << "X_" << node_it-bags.begin()+1 << " ={ ";
        for(vector<int>::const_iterator vertex_it = node_it->begin(); vertex_it != node_it->end(); vertex_it++)
        {
            cout << *vertex_it << " ";
        }
        cout << "}" << endl;
    }
    cout << endl;
}

TreeDecomp::TreeDecomp(string ifname)           //SKIP c lines !!!
{
    ifstream ifs{ifname};
    if (!ifs)
    {
        cerr << "Error: Failed to open .td file '" << ifname << "'\n";
        exit(1); // or return, depending on your function
    }
    string input_holder{};
    //Process first line:               input line: s td numOfBags maxBagWidth n  -> num_of_bags
    getline(ifs,input_holder);
    while(input_holder[0] == 'c')
        getline(ifs,input_holder);
    input_holder = input_holder.substr(0,input_holder.find_last_of(' '));
    input_holder = input_holder.substr(input_holder.find_first_of(' ')+1,input_holder.length()-input_holder.find_first_of(' '));
    input_holder = input_holder.substr(input_holder.find_first_of(' ')+1,input_holder.length()-input_holder.find_first_of(' '));
    input_holder = input_holder.substr(0,input_holder.find_last_of(' '));

    int num_of_bags = stoi(input_holder);

    N.assign(num_of_bags,vector<int>());
    bags.assign(num_of_bags,vector<int>());

    //Process bagcontents:              input line: b bagnumber u v w etc
    int bagIndex = 0;
    while(bagIndex<num_of_bags)
    {
        getline(ifs,input_holder);
        input_holder = input_holder.substr(input_holder.find_first_of(' ')+1,input_holder.length()-input_holder.find_first_of(' '));
        input_holder = input_holder.substr(input_holder.find_first_of(' ')+1,input_holder.length()-input_holder.find_first_of(' '));
        stringstream ss{input_holder};
        int t;
        while(ss >> t)
            bags[bagIndex].push_back(t);
        bagIndex++;
    }

    int source, target;
    while(getline(ifs,input_holder))
    {
        stringstream ss{input_holder};
        ss >> source >> target;
        N[source-1].push_back(target);
        N[target-1].push_back(source);
    }
    ifs.close();
}
