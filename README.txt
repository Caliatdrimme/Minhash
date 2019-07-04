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
- validate on a real data set (have data be read by a process and passed onto element and set processes through messages - unsure of format yet)
- possibly implement a banding candidate pairs finding technique for larger datasets?
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

