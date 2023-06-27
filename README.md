# GenArchBench

GenArchBench is a Genomics benchmark suite targeting the Arm architecture. It comprises 13 multithreaded CPU kernels from the most widely used genomics tools covering the most important genome sequencing steps. GenArchBench includes 10 kernels from [GenomicsBench](https://github.com/arun-sub/genomicsbench) and three additional kernels: the Bit-Parallel Myers algorithm, the Wavefront Alignment algorithm, and a SIMD accelerated version of minimap2's chaining implementation (FAST-CHAIN). The kernels have been ported to RISC-V exploiting, when possible, the RISC-V vector extension.

## Dataset

GenArchBench includes three inputs for each kernel, one tiny with a target execution time of a less than a second, one small with a target execution time of less than a minute, and one large with a target execution time of a couple of minutes. Additionally, we include the expected output for each benchmark and input. To download the dataset (~90 GB):

```
# Small and large inputs
mkdir genarch-temp
cd genarch-temp
wget https://b2drop.bsc.es/index.php/s/Nyg7TXDRpkL5zTn/download
unzip download
rm -r download
cat inputs*/genarch-inputs.tar.gz* > genarch-inputs-merged.bz
rm -r inputs*
tar -xvjf genarch-inputs-merged.bz
mv genarch-inputs ../

# Tiny inputs
wget https://b2drop.bsc.es/index.php/s/AGZCz5XLqEz5sef/download
unzip download
rm -r download
tar -xf inputs_tiny/inputs_tiny.tar.gz -C ../genarch-inputs
cd ..
rm -rf genarch-temp
```

## Working with the benchmarks

Each benchmark under the [benchmark](benchmarks) folder includes a Makefile for compilation and a README that explains how to execute it and some additional information. Additionally, in the `scripts` folder of each benchmark, you will find a wrapper to compile the benchmark using different compilers (`scripts/compile.sh`) and three scripts to automatically run it using its three inputs with different thread counts and automatic output check (`scripts/regression_tiny.sh`, `scripts/regression_small.sh`, and `scripts/regression_large.sh`). In order to use the scripts of each benchmark, you must set the environment variables in [setup.sh](benchmarks/setup.sh), and then run the script: `source benchmarks/setup.sh`.

### Compile

Use the Makefile of each benchmark or the wrapper inside the scripts folder of each benchmark (scripts/compile.sh). The wrapper can be used to compile each kernel with different compilers (by default GCC).

```
cd benchmarks/X
make
# OR
bash scripts/compile.sh
```

### Run

You can follow the instructions of the README inside each benchmark's folder or use our automatic regression tests. 

The regression tests inside the scripts folder of each benchmark, `scripts/regression_tiny.sh`, `scripts/regression_small.sh` and `scripts/regression_large.sh`, run the benchmarks using the tiny, small, and large input, respectively. Note that you need to set the required environment variables in [setup.sh](benchmarks/setup.sh) to use these scripts. 

By default, the regression tests will run the benchmarks three times, each with a different number of threads: 1, 2, and 4. You can change the thread counts by modifying the script. Before finishing, the obtained output will be compared with the expected output to check the correctness of the execution (the output of DGB is not checked by default, take a look at its regression tests). The execution time taken with each thread count and the status of the correctness check will be reported at the end of the execution. The execution time reported only corresponds to the region of interest of the kernels, not the total execution time. 

The regression tests use [run_wrapper.sh](benchmarks/run_wrapper.sh) to automatically detect the job scheduler of the system and run the benchmarks using it. For now, it only supports SLURM and PJM (the job scheduler of the A64FX), but it should be easy to add any other. If [run_wrapper.sh](benchmarks/run_wrapper.sh) does not detect any job-scheduler, it will run the benchmarks without using any.

Example of full workflow:
```
source benchmarks/setup.sh
cd benchmarks/X
bash scripts/compile.sh
bash scripts/regression_tiny.sh
bash scripts/regression_small.sh
bash scripts/regression_large.sh
```

## Licensing

Each benchmark is individually licensed according to the tool it is extracted from.
