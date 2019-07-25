Purpose: to implement a minhash algorithm as described in 

http://infolab.stanford.edu/~ullman/mmds/ch3a.pdf

So far:
- works with fgmpi
- reads in binary strings to represent sets from file example.txt
- creates random hashes (orderings of elements to check)
- creates minhash signatures for sets 
- treats the last listed set as a query set for which returns the best match out of the previous sets
- robust to changes of number of elements, sets, hashes, and lenghts of hashes (provided length of hash <= number of elements)

GRAPH REPRESENTATION

- matches can only occur for a specific hash (so the signatures are originally grouped by hash)
- matches are found and reported to the manager
- the manager creates a graph where the sets are the nodes and the edges are labelled by the hash on which the two sets match (no edge if no match)
- the number of edges between two sets indicates their similarity (take ratio of # of edges to total number of elements)
- the number of edges of one set talks to its similarity/closeness to the rest of the data set (a well-connected set is common/similair to other sets, the least connected set is an outlier)
- can look for clusters and connected components on the graph (in general or on a specific hash)

JACCARD SIMILIARITY
- real similiarity is teh fraction of number of elements that match in a set over the total number of elements in both sets (at least one set has that element)
- can approximate it from the minhash but calculating the fraction of the number of minhash signature matches over the total number of minhashes we imlemented 

TODO: 
- read up on: apriori search; frequency analysis; parallel prefix/scan

- implement the graph representation (for other queries?)
- add capability to answer other queries (outliers etc - based on the graph representatation?)
- refactor/improve readability and code effeciency
- calculate complexity (order of number of messages)

FUTURE TODO's:
- impprove printing (dynamically allocate buffers?)
- design data preprocessing to be able to validate on real data sets
- improve node reusability
- can compare performance and complexity based on num_elem, num_sets, but most importantly on size_hash, num_hash and their interaction


to compile
mpicc sigc.c -o sigs

or
make 

to run 
mpiexec -n N ./sigs num_elem num_sets num_hash size_hash (the other processes are workers)

with fgmpi (2 workers):
mpiexec -nfg 1 -n 22 ./sigs 5 8 4 4 
OR
mpiexec -nfg 22 -n 1 ./sigs 5 8 4 4

Preconditions for command line arguments:
size_hash <= num_elem
num_sets + 2 num_hash + 2 (reader and manager) < total N of processes (the rest are assigned as workers)

