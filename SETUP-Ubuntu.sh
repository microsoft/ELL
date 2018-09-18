#!/bin/bash
set -e

# Minimum version
MIN_PYTHON_VERSION=3.6
MIN_CMAKE_VERSION=3.8.0
MIN_SWIG_VERSION=3.0.12

# Install version
PYTHON_VERSION=$MIN_PYTHON_VERSION
CMAKE_VERSION=3.12.0
SWIG_VERSION=$MIN_SWIG_VERSION

sudo apt-get install software-properties-common

if ! grep -q "^deb .*ubuntu-toolchain-r/test" /etc/apt/sources.list /etc/apt/sources.list.d/*; then
    # Channel for gcc-6 and g++-6
    sudo add-apt-repository ppa:ubuntu-toolchain-r/test
fi

# Update packages
sudo apt-get update && \
sudo apt-get dist-upgrade && \
sudo apt-get install -y gcc-6 g++-6 cmake libedit-dev zlibc zlib1g zlib1g-dev libopenblas-dev doxygen llvm-6.0-dev curl

# Check for cmake version
cmake_ver=$(cmake --version 2>&1 | head -n1 | cut -d ' ' -f3 | awk '{print $NF}')
if [ ! $cmake_ver = $MIN_CMAKE_VERSION ] && printf '%s\n%s\n' $cmake_ver $MIN_CMAKE_VERSION | sort -Cg; then
    curl -o ${HOME}/cmake-$CMAKE_VERSION.tar.gz --location https://cmake.org/files/v$(echo $CMAKE_VERSION | cut -d '.' -f1-2 | awk '{print $NF}')/cmake-$CMAKE_VERSION.tar.gz
    tar zxvf ${HOME}/cmake-$CMAKE_VERSION.tar.gz -C ${HOME}
    pushd ${HOME}/cmake-$CMAKE_VERSION
    ./configure && make && sudo make install
    popd
    rm ${HOME}/cmake-$CMAKE_VERSION.tar.gz
fi

# Check for swig version
swig_ver=$(swig -version 2>&1 | head -n2 | cut -d ' ' -f3 | awk '{print $NF}')
if [ ! $swig_ver = $MIN_SWIG_VERSION ] && printf '%s\n%s\n' $swig_ver $MIN_SWIG_VERSION | sort -Cg; then
    curl -o ${HOME}/swig-$SWIG_VERSION.tar.gz --location http://prdownloads.sourceforge.net/swig/swig-$SWIG_VERSION.tar.gz
    tar zxvf ${HOME}/swig-$SWIG_VERSION.tar.gz -C ${HOME}
    pushd ${HOME}/swig-$SWIG_VERSION
    ./configure --without-pcre && make && sudo make install
    popd
    rm ${HOME}/swig-$SWIG_VERSION.tar.gz
fi

# Check for miniconda and python version
python_ver=$(python --version 2>&1 | head -n1 | cut -d ' ' -f2 | awk '{print $NF}')
if [ ! $python_ver = $MIN_PYTHON_VERSION ] && printf '%s\n%s\n' $python_ver $MIN_PYTHON_VERSION | sort -Cg; then
    sys=$(arch)
    if [ "${sys}" = "x86" ]; then
        curl -o ${HOME}/Miniconda.sh --location https://repo.continuum.io/miniconda/Miniconda3-latest-Linux-x86.sh
    elif [ "${sys}" = "x86_64" ]; then
        curl -o ${HOME}/miniconda.sh --location https://repo.continuum.io/miniconda/Miniconda3-latest-Linux-x86_64.sh
    else
        echo Cannot install miniconda
        exit 1
    fi

    bash ${HOME}/miniconda.sh -b
    rm ${HOME}/miniconda.sh

    echo adding miniconda path to ~/.bashrc
    echo 'export PATH="$HOME/miniconda3/bin:$PATH"' >> ~/.bashrc

    ${HOME}/miniconda3/bin/conda update --update-deps -y -n base conda
    ${HOME}/miniconda3/bin/conda create -n py36 numpy python=$PYTHON_VERSION -y
    source ${HOME}/miniconda3/bin/activate py36
    ${HOME}/miniconda3/bin/conda install -c conda-forge opencv -y
else
    source activate py36
fi

set +echo
