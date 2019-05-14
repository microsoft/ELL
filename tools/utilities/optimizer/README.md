## Model Optimizing

The profiling and optimizing scripts:
1. Take in existing ELL models.
2. Build a suite of model variants for a given target (different convolution type, different convolution parameters, different parallelization, etc.).
3. Run the built variants on that target, collecting profiling information.
4. Using the profiling information, select the best variant and parameters for each layer of the network and produce a new ELL model with those layers.

### Assumptions

The scripts assume:
- You're using python 3
- You're running in a conda environment
- You have `RPI_CLUSTER`, `RPI_APIKEY`, and `RPI_PASSWORD` environment variables set unless you provide those as command line parameters.

## Example Usage
Example 1: Optimizing all the models under a directory on a Raspberry Pi 3. Suppose there are models in `D:\all_models\application_123`, `D:\all_models\application_456`, etc.
```
python profile_and_optimize.py --model_path D:\all_models --target pi3 --output_path optimized_model_output_dir
```

Example 2: Optimizing specific models under a directory on a Rasperry Pi 3. Suppose there are models `D:\all_models\application_123\models\model_name_1.ell`, `D:\all_models\application_456\models\model_name_2.ell`, and , `D:\all_models\application_789\models\model_name_3.ell` and you only want to optimize `model_name_1.ell` and `model_name_2.ell`
```
python profile_and_optimize.py --model_path D:\all_models --models model_name_1 model_name_2 --target pi3 --output_path optimized_model_output_dir
```

## Warnings
- With default options, several hundred profile variants will be built and run per model. On a Xeon 2.4 GHz 2-CPU / 12-core machine with 32 GB RAM using `--parallel_build 8` for 8 build processes and `--parallel_run 48` for 48 concurrent pi3's (see Help below for discussion of these parameters) it can take on the order of 6 hours per model to optimize.
- Since so many profile variants are attempted, these scripts produce an enormous amount of output, so storing output from this script to a log file can take up a lot of disk space (on the order of 25-100 GB per model being optimized with default profile options).

## Profiling Options
The profiling options are specified in a json file and define what profiling configurations should be attempted.

Currently supported options:
```
{
    "convolution_methods": ["simple", "unrolled", "winograd"],
    "parallel": [true, false],
    "winograd_parameters": [...]
}
```
Where `winograd_parameters` is a list of integers that are the values to try for `SeparableBlockDepth`, `NonSeparableFilterBlockDepth`, and `NonSeparableChannelDepth`. Note that all N^3 combinations of `winograd_parameters` will be attempted, so adding values to that will significantly increase the number of models tested. Conversely, if you want to test a smaller space in order to get results faster, removing winograd parameter values will significantly reduce the number of model variants attempted.

## Help:
```
usage: Profile and optimize models for a given target [-h] [--options OPTIONS]
                                                      --model_path MODEL_PATH
                                                      [--models [MODELS [MODELS ...]]]
                                                      [--target TARGET]
                                                      --output_path
                                                      OUTPUT_PATH
                                                      [--temp_dir TEMP_DIR]
                                                      [--concurrent_models CONCURRENT_MODELS]
                                                      [--parallel_build PARALLEL_BUILD]
                                                      [--include_exercise_models]
                                                      [--platform_regex PLATFORM_REGEX]
                                                      [--parallel_run PARALLEL_RUN]
                                                      [--cluster CLUSTER]
                                                      [--ipaddress IPADDRESS]
                                                      [--apikey APIKEY]
                                                      [--username USERNAME]
                                                      [--password PASSWORD]
                                                      [--ell_root ELL_ROOT]
                                                      [--ell_build_root ELL_BUILD_ROOT]
                                                      [--verbosity <LEVEL> | --silence]
                                                      [--logfile <LOG_FILE>]
                                                      [--logmode LOGMODE]

optional arguments:
  -h, --help            show this help message and exit
  --options OPTIONS     Path to options json file specifying profiling options
  --model_path MODEL_PATH
                        Directory containing the models to process
  --models [MODELS [MODELS ...]]
                        Names of models to run
  --target TARGET       Target to cross-compile for
  --output_path OUTPUT_PATH, -o OUTPUT_PATH
                        Path to store optimized models files in
  --temp_dir TEMP_DIR   Directory to store temporary files in
  --concurrent_models CONCURRENT_MODELS
                        How many models to optimize concurrently. Concurrently
                        optimizing models will take more space on the local
                        file system. Builds and runs will still be run
                        parallel regardless of this setting.
  --parallel_build PARALLEL_BUILD
                        The maximum number of build processes to run in
                        parallel.
  --include_exercise_models
                        Build and run the exercise_models executables that
                        don't have profiling enabled
  --platform_regex PLATFORM_REGEX
                        Regex to match test platform name on. Defaults =
                        {'pi3': 'ARMv7 Processor rev 4 \\(v7l\\) \\(4 cores\\)
                        BCM2835', 'pi0': 'ARMv6-compatible processor rev 7
                        \\(v6l\\) \\(1 cores\\) BCM2835'}
  --parallel_run PARALLEL_RUN
                        The maximum number of profile operations to run in
                        parallel. Also the maximum number of devices to
                        attempt to claim.
  --cluster CLUSTER     http address of the cluster server that controls
                        access to the target devices
  --ipaddress IPADDRESS
                        The address of the target device if you don't have a
                        cluster
  --apikey APIKEY       The ApiKey to use for the cluster
  --username USERNAME   The username for pi machine
  --password PASSWORD   The password for pi machines
  --ell_root ELL_ROOT   Path to ELL root.
  --ell_build_root ELL_BUILD_ROOT
                        Path to ELL build directory.
  --verbosity <LEVEL>   Turn on verbosity logging level. Available levels are
                        ['CRITICAL', 'ERROR', 'WARNING', 'INFO', 'DEBUG',
                        'NOTSET']
  --silence             Turn off logging.
  --logfile <LOG_FILE>  Store output to log file.
  --logmode LOGMODE     Set log mode, 'a' or 'w' for append or not append.
```
