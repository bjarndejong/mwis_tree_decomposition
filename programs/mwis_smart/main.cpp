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


using namespace std;

int main(int argc, char* argv[])
{
    //Input
    bool track_solution = false;
    bool store_c = false;
    string filename_td;
    string filename_graph;

    //CLI dealings
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

    //Read treeDecomposition
    TreeDecomp TD(filename_td);
    //Read Graph
    Graph G{filename_graph};


    int start = 1;

    //Solve weighted maximum independent set problem dynamically
    //dfs(start,TD,G);

    RootedTree RT(TD.N,start);
    Smartstorage<__uint128_t> S(G,TD.bags,store_c,track_solution);

    // Create a stringstream to capture std::cout output
    std::stringstream buffer;
    // Save the original std::cout buffer so we can restore it later
    std::streambuf* originalCoutBuffer = std::cout.rdbuf();
    // Redirect std::cout to our stringstream
    std::cout.rdbuf(buffer.rdbuf());

    auto timestamp1 = chrono::high_resolution_clock::now();
    RT.df_traversal(
                    bind(&Smartstorage<__uint128_t>::setup, &S, placeholders::_1),
                    bind(&Smartstorage<__uint128_t>::discover, &S, placeholders::_1, placeholders::_2),      //[](const int&,const RootedTree&){},  //Call it with a dummy lambda option
                    bind(&Smartstorage<__uint128_t>::finish, &S, placeholders::_1, placeholders::_2),  //[](const int&,const RootedTree&){}
                    bind(&Smartstorage<__uint128_t>::cleanup, &S, placeholders::_1)         //[](const RootedTree&){}
                    );
    auto timestamp2 = chrono::high_resolution_clock::now();

    // Step 4: Restore the original std::cout buffer
    std::cout.rdbuf(originalCoutBuffer);
    int capturedValue = 0;
    int capturedPosition = 0;
    buffer >> capturedValue;
    buffer >> capturedPosition;


    cout << capturedValue << endl;
    chrono::duration<double> duration = timestamp2 - timestamp1;

    cout << duration.count() << endl;

    if(track_solution)
    {
    Solution<__uint128_t> MWIS(S,capturedPosition);
    RT.df_traversal(
                    bind(&Solution<__uint128_t>::setup, &MWIS, placeholders::_1),
                    bind(&Solution<__uint128_t>::discover, &MWIS, placeholders::_1, placeholders::_2),//[](const int&, const RootedTree&){},
                    [](const int&, const RootedTree&){},
                    bind(&Solution<__uint128_t>::cleanup, &MWIS, placeholders::_1)                     //[](const RootedTree&){}
                    );
    print_vector(MWIS.MWIS);
    //cout << G.weight_set(MWIS.MWIS) << endl;
    //cout << G.independent_set(MWIS.MWIS) << endl;
    }
    else
    {
        
    }
    return 0;
}