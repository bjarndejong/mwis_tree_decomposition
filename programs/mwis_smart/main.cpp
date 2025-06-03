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


using namespace std;

int main(int argc, char* argv[])
{
    bool track_solution = false;
    bool store_c = false;
    string filename_td;
    string filename_graph;

    vector<string> cliArguments;
    for(int i = 1; i<argc; i++)
        cliArguments.push_back(argv[i]);

    //Check for help
    bool help_was_requested = false;
    for(int i = 0; i<cliArguments.size(); i++)
    {
        if(cliArguments[i]=="--help")
            help_was_requested=true;
    }
    if(help_was_requested)
    {
        if(cliArguments.size()==1)
        {
            cout << "Usage:\n"
            << "  mwis_smart <tree_decomposition.td> <graph.graph> [options]\n\n"
          
            << "Arguments:\n"
            << "  <tree_decomposition.td>     Path to the input tree decomposition file (.td), in PACE 2017 format\n"
            << "  <graph.graph>               Path to the input graph file (.graph)\n\n"
          
            << "  Note: The order of all arguments does not matter. The program will identify\n"
            << "        .td and .graph files based on their file extensions. Flags can be placed anywhere.\n\n"
          
            << "Options:\n"
            << "  --track_solution=true|false   Track and extract the MWIS solution\n"
            << "                                Default: false\n"
            << "  --store_c=true|false          Store intermediate table data (\"C\" values) as temporary .zstd-compressed files\n"
            << "                                for active nodes to reduce memory usage on large tree decompositions.\n"
            << "                                These files are automatically deleted before the program exits.\n"
            << "                                Default: false\n"
            << "  --help                        Show this help message and exit (must be used alone)\n\n"
          
            << "Examples:\n"
            << "  mwis_smart graph.td graph.graph\n"
            << "  mwis_smart graph.graph graph.td --track_solution=true\n"
            << "  mwis_smart --store_c=true graph.graph graph.td\n"
            << "  mwis_smart --track_solution=true --store_c=true tdfile.td gfile.graph\n\n"
          
            << "Requirements:\n"
            << "  The .td file must follow the PACE 2017 format and must be sorted:\n"
            << "    - Bags must be sorted in ascending order by their IDs\n"
            << "    - Each bag's contents must be sorted in ascending order\n"
            << "    - Each edge is represented as a pair (u, v) with u < v\n"
            << "    - The list of edges must be sorted in ascending lexicographical order:\n"
            << "        For edges (u, v) and (i, j),\n"
            << "        (u, v) < (i, j) if u < i, or u = i and v < j\n"
            << "  Unsorted .td files may result in incorrect behavior.\n"
            << "  Consider preprocessing your tree decomposition to ensure sorted order.\n\n"

            << "Output:\n"
            << "  The program prints results in the following format:\n\n"
            << "    <MWIS_weight>\n"
            << "    <time_in_seconds>\n"
            << "    <solution_vertex_list>         (only printed if --track_solution=true)\n\n"
            << "  - <MWIS_weight> is the total weight of the maximum weight independent set.\n"
            << "  - <time_in_seconds> is the time taken to compute the result (floating-point, in seconds).\n"
            << "  - <solution_vertex_list> is a space-separated list of vertex IDs in the MWIS.\n";
            return 0;
        }
        else
        {
            cout << "--help must be the only argument" << endl;
            return 1;
        } 
    }

    //Get td file, make sure exists and unique
    int number_of_td_files = 0;
    for(int i = 0; i<cliArguments.size(); i++)
    {
        if (cliArguments[i].size() >= 3 && cliArguments[i].substr(cliArguments[i].size() - 3) == ".td")
        {
            number_of_td_files++;
            filename_td = cliArguments[i];
        }
    }
    if(number_of_td_files != 1)
    {
        cout << "Arguments must contain a single .td file" << endl;
        return 1;
    }

    //Get graph file, make sure exists and unique
    int number_of_graph_files = 0;
    for(int i = 0; i<cliArguments.size(); i++)
    {
        if (cliArguments[i].size() >= 6 && cliArguments[i].substr(cliArguments[i].size() - 6) == ".graph")
        {
            number_of_graph_files++;
            filename_graph = cliArguments[i];
        }
    }
    if(number_of_graph_files != 1)
    {
        cout << "Arguments must contain a single .graph file" << endl;
        return 1;
    }

    //Check for options --track_solution and --store_c
    if(cliArguments.size()>2 && cliArguments.size()<5)
    {
        int number_of_track_options = 0;
        for(int i = 0; i<cliArguments.size(); i++)
        {
            if(cliArguments[i].size() >= 17 && cliArguments[i].substr(0, 17) == "--track_solution=")
            {
                number_of_track_options++;
                if(cliArguments[i] == "--track_solution=true" || cliArguments[i] == "--track_solution=false")
                {
                    if(cliArguments[i] == "--track_solution=true")
                        track_solution=true;
                    else
                        track_solution=false;
                }
                else
                {
                    cout << "Optional argument --track_solution is set incorrectly" << endl;
                    return 1;
                }
            }
        }
        if(number_of_track_options>1)
        {
            cout << "Can only set --track_solution once" << endl;
            return 1;
        }
        int number_of_store_options = 0;
        for(int i = 0; i<cliArguments.size(); i++)
        {
            if(cliArguments[i].size() >= 10 && cliArguments[i].substr(0, 10) == "--store_c=")
            {
                number_of_store_options++;
                if(cliArguments[i] == "--store_c=true" || cliArguments[i] == "--store_c=false")
                {
                    if(cliArguments[i] == "--store_c=true")
                        store_c=true;
                    else
                        store_c=false;
                }
                else
                {
                    cout << "Optional argument --store_c is set incorrectly" << endl;
                    return 1;
                }
            }
        }
        if(number_of_store_options>1)
        {
            cout << "Can only set --store_c once" << endl;
            return 1;
        }
        if(cliArguments.size()-2 != number_of_track_options+number_of_store_options)
        {
            cout << "Invalid arguments" << endl;
            return 1;
        }
    }
    else
    {
        cout << "Takes at most 4 arguments" << endl;
        return 1;
    }

    //Read treeDecomposition
    //filename_td = argv[1];
    TreeDecomp TD(filename_td);
    //Read Graph
    //filename_graph = argv[2];
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