# Exact MWIS Solver using Tree Decompositions

## Command-Line Usage

```
mwis_smart --help
mwis_smart [options] <tree_decomposition.td> <graph.graph>
```

### Arguments

| Argument                  | Description                                               | Required |
|---------------------------|-----------------------------------------------------------|----------|
| `<tree_decomposition.td>`              | Path to the input tree decomposition file (.td). See [`.td` file](#td-file) below.                    |   Yes    |
| `<graph.graph>`           | Path to the input graph file (.graph). See [`.graph` file](#graph-file) below.                                 |   Yes    |

### Options

| Option                      | Description                                                                                 | Default |
|-----------------------------|---------------------------------------------------------------------------------------------|---------|
| `--track_solution=<true\|false>` | Track and extract the MWIS solution.                                                     | false   |
| `--store_c=<true\|false>`        | Store intermediate data compressed to reduce memory usage.                               | false   |

### Output

The program writes the weight of a maximum weight independent set (MWIS) to standard output:
- Line 1: `<mwis_value>` — the total weight of a MWIS
- Line 2: `<computation_time>` — the time in seconds it took to compute the weight
- Line 3 (optional): If `--track_solution=true` is set, a vertex set is printed in the format:  
  `<mwis>` — a space-separated list of vertex IDs in ascending order

### Example usage

The `.graph` file and `.td` file from this example, together with visualizations (`.png`) of both files, are located in `data/test_set/`.

Running

```
./build/mwis_smart data/test_set/mytest.graph data/test_set/mytest.td --track_solution=true
```
from the root directory of the repository would result in

```
25
0.00047671
1 3 4
```

## File Formats

### `.graph` file
An undirected vertex weighted graph following the METIS format:
- The first line contains three integers: `<number_of_vertices> <number_of_edges> 10`, where   
    - the constant `10` indicates use of vertex weights but not edge weights.
- Vertices are numbered consecutively from `1` to `<number_of_vertices>`.
- Each of the following `<number_of_vertices>` lines describes a vertex `v` by `<weight> <neighbour_1> <neighbour_2> ...` :  
    - Here, `<weight>` is the integer weight of vertex `v`, followed by a space-separated list of its neighbours in ascending order.

### `.td` file
The tree decomposition must follow the PACE 2017 format with additional ordering and sorting requirements as specified below:
- **Line types**:
    - `c` — *comment lines*, e.g., `c <text>`
    - `s` — *solution line*, first non-comment line, format:  
    `s td <number_of_bags> <max_bag_size> <number_of_vertices>`
    - `b` — *bag lines*, format:  
    `b <bag_ID> <bag_contents>`
    - *Tree edge lines*, format:  
    `<bag_ID_1> <bag_ID_2>`, where `<bag_ID_1> < <bag_ID_2>`
- **General constraints**:
    - Bag IDs are continuous integers from `1` to `<number_of_bags>`.
    - The solution line (`s`) must be the first non-comment line; bag lines (`b`) must appear after the solution line and before the tree edge lines.
- **Additional constraints**:
    - Comment lines (`c`) appear only at the beginning of the file.
    - Bag lines must be sorted by bag ID in ascending order.
    - Bag contents within each bag line must be sorted in ascending order.