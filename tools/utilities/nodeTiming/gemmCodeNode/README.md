The `nodeTiming/gemmCodeNode` directory contains tools for building ELL models with different Matrix-Matrix multiplication implementations and measuring the performance of those implementations.

Layout of gemmCodeNode:

```
deploy/                                 -- contains files to be copied without alteration into implementation timing directories
    full_pass.(cmd|sh)                  -- runs import_all.(cmd|sh), build_all.(cmd|sh), and run_all.(cmd|sh) in a timing directory
    run.py -c N                         -- runs run_all.(cmd|sh) N times (default 1) and runs the timing_aggregator processing on the results
    timing_aggregator.py -f filename    -- reads filename and parses for gemm time output. For each uniquely named gemm impl it sees it will aggregate those times and report the range, average, and ratio of average time against the first implementation that it sees

scripts/                                -- contains scripts for building ELL models with various GEMM impls and generating the test projects for building, running, and timing the performance of the models
    build_gemm_models.py                -- builds ELL models with specified GEMM implementation and panel/kernel parameters
    make_default_models.py              -- builds ELL models for all of the different GEMM implementations and the OpenBLAS, naive for-loop, and (if -mkl specified and MKL is installed) the MKL implementation
    special_model_args.py               -- contains a dictionary mapping model file name to a list of additional arguments to pass to wrap.py for that model when importing it
    build_tests.py                      -- generates the testing and timing projects for the given models and implementations (specified with the -v option)

src/                                    -- contains source files and template source files for the testing and timing projects
    CMakeLists.txt.in                   -- CMakeLists.txt template file for the timing project. This is read and CMakeLists.txt is produced by scripts/build_tests.py.
    Runner.cpp.in                       -- Runner.cpp template file for the timing project. This generates the main cpp file for model running and timing. This is read and CMakeLists.txt is produced by scripts/build_tests.py.
```

General workflow with these tools:
```
# Modify and introduce a new GEMM implementation
<< build ELL >>

cd <ELLROOT>/tools/utilities/gemmCodeNode

# Generate ELL models for each GEMM implementation in the ./models/ directory
# This only needs to be done whenever a new GEMM implementation is added, but not when an existing implementation is modified
python scripts/make_default_models.py

# Generate the testing projects for the GEMM implementations being tested
# Suppose implementations 2, 5, 7, and 14 are being tested and the directory to put the test projects in is named my_testing_dir
# To include mkl, provide the -mkl flag
python scripts/build_tests.py -v 2 5 7 14 -o my_testing_dir

# cd into the test directory for the data type and size that you want to test
cd my_testing_dir/float/256x256

# Run import_all, build_all, and run_all:
./full_pass.(cmd|sh)

# To get aggregate timing over 20 runs:
python ./run.py -c 20
```