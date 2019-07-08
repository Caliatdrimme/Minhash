Purpose: to implement a minhash algorithm as described in 

http://infolab.stanford.edu/~ullman/mmds/ch3a.pdf

So far:
- works with fgmpi
- randomly creates binary strings to represent sets
- creates random hashes (orderings of elements to check)
- creates minhash signatures for sets 
- returns candidate pairs (that have overlap of minhash signatures > threshold)
- robust to changes of number of elements, sets, hashes, and lenghts of hashes (provided length of hash <= number of elements)

TODO: 
- validate on a real data set
- http://corpus-texmex.irisa.fr/ SIFT1B (92 GB dataset - 1 billion vectors)
- https://nlp.stanford.edu/projects/glove/ Common crawl 840B tokens (2GB dataset - 2.2 million vocab) 
- possibly implement a banding candidate pairs finding technique for larger datasets
- refactor/improve readability and code effeciency/make it scalable to size


to compile
mpicc sigc.c -o sigs

or
make 

to run 
mpiexec -n N ./sigs num_elem num_sets size_hash

simple example mpi:
mpiexec -n 13 ./sigs 4 4 4

fgmpi:
mpiexec -n 1 -nfg 13 ./sigs 4 4 4
OR
mpiexec -n 13 -nfg 1 ./sigs 4 4 4

Preconditions for command line arguments:
size_hash <= num_elem
num_elem + num_sets < total N of processes (the rest are assigned as hashes)

