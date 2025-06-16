## Command-Line Usage

```bash
mwis_smart --help
mwis_smart [options] <.td file> <.graph file>
```

### Arguments

| Argument                  | Description                                               | Required |
|---------------------------|-----------------------------------------------------------|----------|
| `<.td file>`              | Path to input tree decomposition file                     |   yes    |
| `<.graph file>`           | Path to input graph file                                  |   yes    |

### File Formats


- **`.graph` file (Input Graph):**  
  The file format is as follows:  
  - The first line contains three integers: `n m 10`  
    - `n` = number of vertices  
    - `m` = number of edges  
    - The constant `10` (format identifier)  
  - Vertices are numbered consecutively from 1 to `n`
  - Each of the following `n` lines describes a vertex:  
    - The line contains the vertex weight `w(v)` followed by the vertex’s neighbors `N(v)`  
    - Example: `7 9 10` means vertex weight 7, neighbors vertices 9 and 10  

- **`.td` file (Input Tree Decomposition):**  
  Must follow the PACE 2017 format and be sorted as follows:  
  - Bags sorted ascending by their IDs  
  - Each bag’s contents sorted in ascending  
  - Edges between bags listed as pairs of bag IDs separated by space, e.g., `u v` with `u < v`  
  - The list of edges sorted lexicographically, i.e., `u v < i j` if `u < i`, or if `u = i` then `v < j`  
  Unsorted `.td` files may cause incorrect behavior. Preprocessing to ensure sorted order is recommended.


### Options

| Option                      | Description                                                                                 | Default |
|-----------------------------|---------------------------------------------------------------------------------------------|---------|
| `--track_solution=true\|false` | Track and extract the MWIS solution.                                                     | false   |
| `--store_c=true\|false`        | Store intermediate data compressed to reduce memory usage.                               | false   |

### Example

The `.graph` file and `.td` file from this example, together with visualizations (`.png`) of both files, are located in `data/test_set/`.

Running

```bash
./build/mwis_smart data/test_set/mytest.graph data/test_set/mytest.td --track_solution=true
```
from the root directory of the repository would result in

```
25
0.00047671
1 3 4 
```