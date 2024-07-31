# Stigmer simulation

## Overview

This stochastic agent-based model simulates human behavior in the Stigmer game, where agents interact within an environment, modifying it and responding to its changes. The simulation allows for the analysis of various strategies and behaviors, and to the determination of optimal strategies or strategies mimicking the one of humans in the real-life experiment.

## Build and run the simulation

In the terminal navigate to the project directory. Replace `PATH_TO_PROJECT` with the actual path of the project:
```bash
$ cd PATH_TO_PROJECT/
```

Use CMake to configure and build the project:
```bash
$ cmake -B ./build/ -S ./
$ make -C ./build/
```

Execute the compiled binary to run the simulation. Replace `NAME_OF_COMPILED_FILE` with the actual name of your compiled file:
```bash
$ ./build/NAME_OF_COMPILED_FILE
```

To clean the build directory, removing all compiled files:
```bash
$ cmake --build ./build/ --target clean
```
## Examples

This program contains two examples demonstrating different use cases of the simulation:
- **Observables Calculation** (`main_obs.cpp`). This example shows how to use the program to calculate and save various observables from the simulation. It initializes the game, runs the simulation, and analyzes the results.
- **Monte Carlo Simulation** (`main_MC.cpp`). This example demonstrates how to use the program to find agents whose behavior closely matches that of humans using a Monte Carlo method. It involves running multiple simulations with different parameters and selecting the best-fitting agents.
