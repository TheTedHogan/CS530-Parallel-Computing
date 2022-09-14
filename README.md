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

This will output an executable file named `CS530_Parallel_Computing` into the build directory that can then be run 
from the command line, for example

`./CS530_Parallel_Computing`


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

