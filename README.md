# Stigmer simulation

## Run simulation

In the terminal write
```bash
$ cd PATH_TO_FOLDER/simulations/
```
```bash
$ cmake -B ./build/ -S ./ && make -C ./build/
```
```bash
$ ./build/Stigmer_simulation_obs_ref
```

To clean the build folder:
```bash
$ cmake --build ./build/ --target clean
```
