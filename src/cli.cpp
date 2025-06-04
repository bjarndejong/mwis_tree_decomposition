#include "cli.h"

#include <iostream>
#include <string>
#include <vector>

using namespace std;

//Check for options --track_solution and --store_c
int check_for_options(const vector<string>& cliArguments, bool& track_solution, bool& store_c)
{
    if(cliArguments.size()>=2 && cliArguments.size()<5)
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
    return 0;
}

//Get td file, make sure exists and unique
int check_for_td_file(const vector<string>& cliArguments, string& filename_td)
{
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
    else 
        return 0;
}

//Get graph file, make sure exists and unique
int check_for_graph_file(const vector<string>& cliArguments, string& filename_graph)
{
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
    else
        return 0;
}

//Check for help
int check_for_help(const vector<string>& cliArguments)
{
    bool help_was_requested = false;
    for(int i = 0; i<cliArguments.size(); i++)
    {
        if(cliArguments[i]=="--help")
            help_was_requested = true;
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
            << "    - Each edge between bags must be listed as two bag IDs separated by a space:\n"
            << "        u v   (with u < v)\n"
            << "    - The list of edges must be sorted in ascending lexicographical order:\n"
            << "        For edges u v and i j,\n"
            << "        u v < i j if u < i, or u == i and v < j\n"
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
    else
        return 2;
}