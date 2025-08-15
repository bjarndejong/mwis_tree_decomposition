#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <functional>
#include <chrono>

#include "../../src/general.h"
#include "../../src/cli.h"
#include "../../src/graph.h"
#include "../../src/treedecomp.h"
#include "../../src/rootedtree.h"
#include "../../src/smartstorage.h"
#include "../../src/solution.h"



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

    const int start = 1;
    RootedTree RT(move(TD.N),start);
    
    Smartstorage<__uint128_t> S(G,TD.bags,store_c,track_solution);

    // Create a stringstream to capture std::cout output
    // Store the original cout buffer to restore later
    // Redirect cout to stringstream
    //stringstream buffer;
    //streambuf* originalCoutBuffer = cout.rdbuf();
    //cout.rdbuf(buffer.rdbuf());

    auto timestamp1 = chrono::high_resolution_clock::now();
    RT.df_traversal(
        bind(&Smartstorage<__uint128_t>::setup, &S, placeholders::_1),
        bind(&Smartstorage<__uint128_t>::discover, &S, placeholders::_1, placeholders::_2),
        bind(&Smartstorage<__uint128_t>::finish, &S, placeholders::_1, placeholders::_2),  //[](const int&,const RootedTree&){} -- lambda dummy
        bind(&Smartstorage<__uint128_t>::cleanup, &S, placeholders::_1)
    );
    auto timestamp2 = chrono::high_resolution_clock::now();
    //Restore cout
    //cout.rdbuf(originalCoutBuffer);
    
    int capturedValue = 0;
    int capturedPosition = 0;
    //buffer >> capturedValue;
    //buffer >> capturedPosition;


    //cout << capturedValue << endl;
    chrono::duration<double> duration = timestamp2 - timestamp1;

    //cout << duration.count() << endl;
    /*
    if(track_solution)
    {
        Solution<__uint128_t> MWIS(S,capturedPosition);
        RT.df_traversal(
            bind(&Solution<__uint128_t>::setup, &MWIS, placeholders::_1),
            bind(&Solution<__uint128_t>::discover, &MWIS, placeholders::_1, placeholders::_2),//[](const int&, const RootedTree&){} -- lambda dummy
            [](const int&, const RootedTree&){},
            bind(&Solution<__uint128_t>::cleanup, &MWIS, placeholders::_1)
        );
        print_vector(MWIS.MWIS);
        //cout << G.weight_set(MWIS.MWIS) << endl;
    }
    */
    return 0;
}