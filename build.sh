#! /bin/
# get path of current script: https://stackoverflow.com/a/39340259/207661
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

set -e
set -x

# make sure we have the required tools
if printf '%s\n%s\n' "$(cmake --version | head -n1 | cut -d" " -f3 | awk '{print $NF}')" 3.7.9 | sort -CV; then
    echo "Your cmake version is less than the required 3.8.0"
    echo "See INSTALL-Ubuntu.md for information on how to install cmake"
    exit 1
fi

if printf '%s\n%s\n' "$(gcc --version | head -n1 | cut -d" " -f4 | awk '{print $NF}')" 5.0.0 | sort -CV; then
    echo "Your GCC compiler version is less than the required 5.0.0"
    echo "See INSTALL-Ubuntu.md for information on how to install GCC"
    exit 1
fi

if printf '%s\n%s\n' "$(llc --version | grep -e 'LLVM version' | cut -d" " -f5 | awk '{print $NF}')" 3.9.0 | sort -CV; then
    echo "Your LLVM version is less than the required 3.9.1"
    echo "See INSTALL-Ubuntu.md for information on how to install llvm"
    exit 1
fi

if printf '%s\n%s\n' "$(swig -version | grep -e 'SWIG Version' | cut -d" " -f3 | awk '{print $NF}')" 3.0.11 | sort -CV; then
    echo "Your swig version is less than the required 3.0.12"
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

pushd "$SCRIPT_DIR"  >/dev/null
# variable for build output
build_dir=build
echo "putting build in $build_dir folder, to clean, just delete the directory..."

if [[ ! -d $build_dir ]]; then
    mkdir -p $build_dir
fi

pushd $build_dir  >/dev/null
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j
make -j _ELL_python
popd >/dev/null
