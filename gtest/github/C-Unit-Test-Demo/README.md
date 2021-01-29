# C-Unit-Test-Demo
A project for demonstrating how to test C code with Google Test.

## Prerequisites

You need to install the following software in order to run this project

- [git](https://git-scm.com/)
- A C++14-compatible compiler (gcc or clang). Under Linux gcc is usually already installed by default.
- [CMake](https://cmake.org/) v3.5 or higher (install via `sudo apt install cmake`)
- [Google Test](https://github.com/google/googletest) (Installation instructions are given on the GitHub pages)


## Building

1. Create a project folder: 

```
cd ~
mkdir unit-test-demo
cd unit-test-demo
```

2. Clone this repository with git: `git clone https://github.com/Jawan81/C-Unit-Test-Demo.git`
3. Create a build output directory:


```
mkdir build
cd build
```

4. Run CMake to create the makefiles: `cmake ../C-Unit-Test-Demo`
5. Either 
 - build all build targets by just calling `make` 
 - or choose your build target
    - For building the unit tests run: `make runUnitTests` 
    - for the example executable run: `make runMain`
6. Run the executables
 - For the unit tests run `tests/runUnitTests` 
 - For the example application run `src/runMain`
