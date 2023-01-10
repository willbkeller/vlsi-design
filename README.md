# VLSI Design Programs
The included folders contain progrrams to run Circuit Simulation and Circuit Bipartion

#### Circuit Simulation:
* The code runs a simulation and gives an output based on the circuit selected for simulation. The .isc files are the circuit description and .vec files are the corresponding input vectors.
* To run the code, a make file is included which will create and executable titled 'project'.
* Command line input to run simulation (Linux/Mac): *./project [.isc file] [.vec file]* (i.e. ./project ../isc_files/c880.isc ../vec_files/c880.vec)
* Output will be in a file writen in isc_files
  
#### Circuit Bipartition:
* The circuit bipartition code uses the Fiduccia-Mattheyses Algorithm to find the optimum partition of the passed in circuits.
* Circuit descriptions can be found in the benchmarks folder. The src folder contains a make file that creates an executable titled 'project'
* Command line inpute to run bipartition algorithm (Linux/Mac): *./project [benchmark file]* (i.e. ./project ../benchmarks/c7552.bench)
* Output is displayed in terminal providing the number of passes, the best cut per pass, and the final cut with runtime
  
