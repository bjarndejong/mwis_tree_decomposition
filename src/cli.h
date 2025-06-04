#ifndef CLI_H_INCLUDED
#define CLI_H_INCLUDED

#include <vector>
#include <string>

int check_for_help(const std::vector<std::string>& cliArguments);

int check_for_td_file(const std::vector<std::string>& cliArguments, std::string& filename_td);

int check_for_graph_file(const std::vector<std::string>& cliArguments, std::string& filename_graph);

int check_for_options(const std::vector<std::string>& cliArguments, bool& track_solution, bool& store_c);

#endif // CLI_H_INCLUDED