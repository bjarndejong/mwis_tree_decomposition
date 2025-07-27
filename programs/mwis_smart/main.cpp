#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <functional>
#include <numeric>
#include <algorithm>
#include <bit>
#include <chrono>
#include <stdint.h>
#include <stdio.h>
#include <unordered_map>

#include "../../src/treedecomp.h"
#include "../../src/graph.h"
#include "../../src/rootedtree.h"
#include "../../src/solution.h"
#include "../../src/general.h"
#include "../../src/smartstorage.h"
#include "../../src/cli.h"
#include "../../src/binaryinteger.h"



using namespace std;

int main(int argc, char* argv[])
{
    //Input
    bool track_solution = false;
    bool store_c = false;
    string filename_td;
    string filename_graph;

    //CLI dealings
    //
    vector<string> cliArguments;
    for(int i = 1; i<argc; i++)
        cliArguments.push_back(argv[i]);

    //Check for help
    int helpStatus = check_for_help(cliArguments);
    if(helpStatus < 2)
        return helpStatus;

    //Get td file, make sure exists and unique
    int tdStatus = check_for_td_file(cliArguments,filename_td);
    if(tdStatus==1)
        return tdStatus;

    int graphStatus = check_for_graph_file(cliArguments,filename_graph);
    if(graphStatus==1)
        return graphStatus;

    //Check for options --track_solution and --store_c
    int optionStatus = check_for_options(cliArguments,track_solution,store_c);
    if(optionStatus==1)
        return optionStatus;

    //Read and store .graph and .td
    TreeDecomp TD = TreeDecomp::from_file(filename_td);
    Graph G = Graph::from_file(filename_graph);

    size_t max_bag_size = 0;
    for(size_t i = 0; i<TD.bags.size(); i++)
        max_bag_size = max(max_bag_size,TD.bags[i].size());
    
    BinaryInteger::set_number_of_blocks(max_bag_size);

    const int start = 1;
    RootedTree RT(TD.N,start);

    Smartstorage<BinaryInteger> S(G,TD.bags,store_c,track_solution);

    // Create a stringstream to capture std::cout output
    // Store the original cout buffer to restore later
    // Redirect cout to stringstream
    stringstream buffer;
    streambuf* originalCoutBuffer = cout.rdbuf();
    cout.rdbuf(buffer.rdbuf());

    auto timestamp1 = chrono::high_resolution_clock::now();
    RT.df_traversal(
        bind(&Smartstorage<BinaryInteger>::setup, &S, placeholders::_1),
        bind(&Smartstorage<BinaryInteger>::discover, &S, placeholders::_1, placeholders::_2),
        bind(&Smartstorage<BinaryInteger>::finish, &S, placeholders::_1, placeholders::_2),  //[](const int&,const RootedTree&){} -- lambda dummy
        bind(&Smartstorage<BinaryInteger>::cleanup, &S, placeholders::_1)
    );
    auto timestamp2 = chrono::high_resolution_clock::now();
    //Restore cout
    cout.rdbuf(originalCoutBuffer);
    
    int capturedValue = 0;
    int capturedPosition = 0;
    buffer >> capturedValue;
    buffer >> capturedPosition;


    cout << capturedValue << endl;
    chrono::duration<double> duration = timestamp2 - timestamp1;

    cout << duration.count() << endl;

    if(track_solution)
    {
        Solution<BinaryInteger> MWIS(S,capturedPosition);
        RT.df_traversal(
            bind(&Solution<BinaryInteger>::setup, &MWIS, placeholders::_1),
            bind(&Solution<BinaryInteger>::discover, &MWIS, placeholders::_1, placeholders::_2),//[](const int&, const RootedTree&){} -- lambda dummy
            [](const int&, const RootedTree&){},
            bind(&Solution<BinaryInteger>::cleanup, &MWIS, placeholders::_1)
        );
        print_vector(MWIS.MWIS);
    }
    return 0;
}