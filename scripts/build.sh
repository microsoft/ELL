#!/bin/bash

# get path of current script: https://stackoverflow.com/a/39340259/207661
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && cd .. && pwd )"

# usage: build.sh build_dir nproc dailyclean cmakeargs
# This script checks to make sure you have required tools instealled then builds ELL.
# Options:
#     build_dir  - the output folder (default 'build')
#     nproc      - number of procs to use (default let make decide)
#     dailyclean - true/false if you want the folder cleaned each day
#     cmakeargs  - args to pass cmake (default '-DCMAKE_BUILD_TYPE=Release ..')

set -e

# make sure we have the required tools
if printf '%s\n%s\n' "$(cmake --version | head -n1 | cut -d" " -f3 | awk '{print $NF}')" 3.7.9 | sort -CV; then
    echo "Your cmake version is less than the required 3.8.0"
    echo "See INSTALL-Ubuntu.md for information on how to install cmake"
    exit 1
fi

if printf '%s\n%s\n' "$(gcc --version | head -n1 | cut -d" " -f4 | awk '{print $NF}')" 8.0.0 | sort -CV; then
    echo "Your GCC compiler version is less than the required 8.0.0"
    echo "See INSTALL-Ubuntu.md for information on how to install GCC"
    exit 1
fi

if [[ ! -f "/usr/bin/llvm-config-8" ]]; then
    echo "LLVM version 8 needs to be installed"
    echo "See INSTALL-Ubuntu.md for information on how to install llvm"
    exit 1
fi

if printf '%s\n%s\n' "$(swig -version | grep -e 'SWIG Version' | cut -d" " -f3 | awk '{print $NF}')" 3.0.12 | sort -CV; then
    echo "Your swig version is less than the required 4.0.0"
    echo "See INSTALL-Ubuntu.md for information on how to install swig"
    exit 1
fi

if printf '%s\n%s\n' "$(python --version 2>&1 | head -n1 | cut -d" " -f2 | awk '{print $NF}')" 3.5.9 | sort -CV; then
    echo "Your python version is less than the required 3.6"
    echo "See INSTALL-Ubuntu.md for information on how to install python using anaconda"
    exit 1
fi


if [[ ! -f "/usr/lib/libopenblas.a" ]]; then
    sudo apt-get install -y libopenblas-dev
fi

pushd "$SCRIPT_DIR" > /dev/null
# variable for build output
build_dir=$1
if [[ -z "$build_dir" ]]; then
    build_dir=build
else
    shift
fi

nproc=$1
if [[ ! -z "$nproc" ]]; then
    shift
fi
dailyclean=$1
if [[ ! -z "$dailyclean" ]]; then
    shift
fi

incremental="true"

if [[ -d $build_dir ]]; then
    if [ "${dailyclean}" = "true" ]; then
        d="$(date +%d/%m/%Y)"
        e=""
        if [[ -f .lastbuild ]]; then
            e="$(cat .lastbuild)"
        fi
        if [ ! "${d}" = "${e}" ]; then
            echo forcing clean build by deleting ${build_dir}
            rm -rf $build_dir
            incremental="false"
        fi
        echo ${d} > .lastbuild
    fi
    if [ "${incremental}" = "true" ]; then
        echo performing incremental build in ${build_dir}
    fi
fi

if [[ ! -d $build_dir ]]; then
    echo "putting build in $build_dir folder, to clean, just delete the directory..."
    mkdir -p $build_dir
fi

set -x

cmakeargs="$@"
if [[ -z "${cmakeargs}" ]]; then
    cmakeargs="-DCMAKE_BUILD_TYPE=Release .."
fi

pushd $build_dir > /dev/null
cmake ${cmakeargs}
make -j $nproc
make -j $nproc _ELL_python
popd > /dev/null
