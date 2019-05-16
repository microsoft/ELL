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

TMPDIR=`mktemp -d`

sudo apt-get install software-properties-common

if ! grep -q "^deb .*ubuntu-toolchain-r/test" /etc/apt/sources.list /etc/apt/sources.list.d/*; then
    # Channel for gcc-8 and g++-8
    sudo add-apt-repository ppa:ubuntu-toolchain-r/test
fi

echo === Adding LLVM 8 package repositories ===
sudo sh -c 'echo deb http://apt.llvm.org/bionic/ llvm-toolchain-bionic-8 main >> /etc/apt/sources.list'
sudo sh -c 'echo deb-src http://apt.llvm.org/bionic/ llvm-toolchain-bionic-8 main >> /etc/apt/sources.list'
wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -

echo === Update packages ===
sudo apt-get update -y
sudo apt-get install -y gcc-8 g++-8 cmake libedit-dev zlibc zlib1g zlib1g-dev libopenblas-dev doxygen llvm-8 curl make portaudio19-dev

set +e
cmake_ver=$(cmake --version 2>&1 | head -n1 | cut -d ' ' -f3 | awk '{print $NF}')
cmake_path=$(which cmake)
set -e
if [[ "$cmake_path" == "" ]] ; then
   cmake_ver=0
fi
echo === Check cmake version, found $cmake_ver ===
if [[ $(python tools/utilities/pythonlibs/version.py $cmake_ver $MIN_CMAKE_VERSION) ]]; then
    echo "Downloading and installing CMake"
    cmake_short_version=$(echo ${CMAKE_VERSION} | cut -d '.' -f1-2 | awk '{print $NF}')
    cmake_installer_file="cmake-${CMAKE_VERSION}-Linux-x86_64.sh"
    curl -o "${TMPDIR}/${cmake_installer_file}" --location "https://cmake.org/files/v${cmake_short_version}/${cmake_installer_file}"
    pushd "${TMPDIR}"
    sudo sh "./${cmake_installer_file}" --skip-license --prefix=/usr --exclude-subdir
    popd
fi

set +e
swig_ver=$(swig -version 2>&1 | head -n2 | cut -d ' ' -f3 | awk '{print $NF}')
swig_path=$(which swig)
set -e
if [[ "$swig_path" == "" ]] ; then
   swig_ver=0
fi
echo === Check swig version, found $swig_ver ===
if [[ $(python tools/utilities/pythonlibs/version.py $swig_ver $MIN_SWIG_VERSION) ]]; then
    echo "Downloading and building SWIG to ${TMPDIR}"
    curl -o "${TMPDIR}/swig-${SWIG_VERSION}.tar.gz" --location "http://prdownloads.sourceforge.net/swig/swig-${SWIG_VERSION}.tar.gz"
    pushd "${TMPDIR}"
    tar zxvf "swig-${SWIG_VERSION}.tar.gz"
    pushd "swig-${SWIG_VERSION}"
    ./configure --without-pcre && make && sudo make install
    popd
    popd
fi

python_ver=$(python --version 2>&1 | head -n1 | cut -d ' ' -f2 | awk '{print $NF}')
echo === Check miniconda and python version, found $python_ver ===
if [ ! -d ${HOME}/miniconda3 ] ; then
    echo "Downloading and installing miniconda"
    sys=$(arch)
    if [ "${sys}" = "x86" ]; then
        curl -o "${TMPDIR}/Miniconda.sh" --location https://repo.continuum.io/miniconda/Miniconda3-latest-Linux-x86.sh
    elif [ "${sys}" = "x86_64" ]; then
        curl -o "${TMPDIR}/miniconda.sh" --location https://repo.continuum.io/miniconda/Miniconda3-latest-Linux-x86_64.sh
    else
        echo "Cannot install miniconda"
        exit 1
    fi

    bash "${TMPDIR}/miniconda.sh" -b
    rm "${TMPDIR}/miniconda.sh"

    echo "Adding miniconda path to ~/.bashrc"
    echo 'export PATH="$HOME/miniconda3/bin:$PATH"' >> ~/.bashrc

    ${HOME}/miniconda3/bin/conda update --update-deps -y -n base conda
    ${HOME}/miniconda3/bin/conda create -n py36 pip python=$PYTHON_VERSION -y

fi

echo === Activating py36 conda environment ===
source ${HOME}/miniconda3/bin/activate py36
pip install -r requirements.txt

rm -rf "${TMPDIR}"
