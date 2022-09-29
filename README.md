# CS530-Parallel-Computing

## System Requirements

  * A C compiler, this repo targets gcc
  * cmake
    * https://cmake.org/install/
    * On mac, this can be installed with `brew install cmake`
  * make

## Build and Run Instructions

This project is currently set to be built using cmake.

To build the project, first create a build directory, on *nix

`mkdir build`

Next, enter the newly created directory

`cd build`

From the build directory, run cmake and pass the top level directory as the argument

`cmake ..`

Next, run make from inside the build directory

`make`

This will output an executable file with a name corresponding to the function into the build directory that can then be run
from the command line, for example

`./fibonacci 10 s`

### Running Fibonacci
There are two fibonacci programs. The first is a serial program that implements a recursive algorithm to calculate the sequence
The fibonacci program takes one mandatory and one optional argument. The first argument required is the number in the fibonacci you would like to the display the result for. The second argument is option to display the intermediate values in the series as well.

`./fibonacci 10 s`

The second fibonacci program uses Binets algorithm. When the entire sequence is requested using the `s` flag, it is parallelized using Binet's algorithm.
It can be called with the following command:

`./fibonacci_omp 10 s`


### Running Liebniz

The leibniz program takes one required argument, the number of iterations, and will output the approximation of pi.

`./leibniz 10`

### Running Monte Carlo

The monte carlo program takes one required argument, the number of iterations, and will output the approximation of pi.

`./montecarlo 10`

### Running Matrix Vector Multiply
The matrixvector multiply will compute the product of a matrix and vector that are supplied in mmio coordinate form. It will also output the results to an mmio file.
The output file will be overwritten.

To run the program, the following command and arguments are used
`./matrixvector [matrix source file] [vector source file] [outputfile]'`
There are example matrix and vector mmio .mtx files in the /etc directory

### Running Matrix Matrix Multiply
The matrixvector multiply will compute the product of two matrices that are supplied in mmio coordinate form. The matrices must be of compatible shape.
It will also output the results to an mmio file.
The output file will be overwritten.

To run the program, the following command and arguments are used
`./matrixmatrix [MatrixA source file] [MatrixB source file] [outputfile]'`
There are example matrix and vector mmio .mtx files in the /etc directory

## Executing Tests
There is an executable assocated with each program, with the same name as the program with '_test' appended to it. To run the tests for a program, simply execute the program with no arguments.
The test files themselves are housed in the src folders next to the program files they test.

## Editor Config
Attached in the root of this repo is a `.editorconfig` file which will help us to standardize our coding styles to a degree.

Different IDE's/TextEditor's may require the installation of an extension in order to properly make use of the `.editorconfig` file

- ### VSCode Instructions
    - Install [this extension](https://marketplace.visualstudio.com/items?itemName=EditorConfig.EditorConfig) inside VSCode and reload the project

## Decisions for Leibniz series
- The Leibniz series is fairly simple as the only input needed it the `#` of iterations desired. running `./leibniz_series #` where `#` is a positive integer will provide and approximation of pie. The larger the `#` the better the approximation to further decimal levels.


## Decisions for the Monte Carlo
- The Monte Carlo approximations is fairly simple as the only input needed is the `#` of iterations desired. running `./monte_carlo #` where `#` is a positive integer will provide and approximation of pie. The larger the `#` the better the approximation to further decimal levels.

## Decisions Made For Fibonacci
- Decided to make this a recursive function as opposed to a for-loop style funciton, this made the code much simpler and captures more accurately the essence of the fibonacci sequence
- Decided to allow for two different types of output. The first type of output simply returns an integer which is the n-th value in the sequence. The value for `n` is provided by the user in an argument eg. `./fibonacci 4`. The second type of out put prints out the entire sequence up to the n-th value in the fibonacci sequence. The user can denote they want the entire sequence by appending the argument `s` at the end of the command eg. `./fibonacci 4 s`

## Decisions made for MatrixVector and MatrixMatrix
- The file format for matrices and vector was determined to be mmio / mtx files that are in coordinate forms. The decision to support coordinate forms and not other forms was due to the flexibility of the format, able to represent both dense and sparse matrices, and the availability of documentation on that specific format.
- The decision to output the results to a file only was made as any large size matrix would be difficult to understand on stdout.
