#!/bin/bash
# Set Color Codes
RED='\033[0;31m'
GREEN='\033[0;32m'
DEFAULT='\033[0m'

# Check For APT
CHECKAPT="$(whereis apt-get | awk '{print $2}')"
# Check If Python3 Is Installed
CHECKPYTHON3="$(whereis python3 | awk '{print $2}')"
# Get Excate Python Version
PYTHON3VERSION="$(python3 -c 'import platform; print(platform.python_version())' |  sed 's/+//g')"
# Set Required Python Version
REQUIREDPYTHON3="3.6"

if [ -f $CHECKPYTHON3 ]; then
        echo "Python 3 Installed Checking Version Is 3.6 Or Greater"

        COMPARE="$(echo "" | awk '{print ( '$PYTHON3VERSION' >= '$REQUIREDPYTHON3' )}')"
        if [ $COMPARE -eq 1 ];
        then
        echo "$GREEN Python 3.6 Or Greater Installed $DEFAULT";
        else
        echo "$RED Python 3.6 Or Greater Not Installed $DEFAULT"
        echo "Please Install It Manually"
        echo "Now Installing Other Tools In 5 Secods"
        sleep 5s
        fi

else
        echo "Python 3 Not Installed"
        echo "Now Installing"
        apt-get install python3.6 -y
fi

# Check For APT As It Is Faster Than APT-GET For Large Installs
if [ ! -f $CHECKAPT ]; then
        echo "APT not found! - Falling Back To APT-GET"
        # Not auto updating incase some users want to use old software
        apt-get update
        wget -O - http://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -
        apt-add-repository "deb http://apt.llvm.org/xenial/ llvm-toolchain-xenial-3.9 main"
        # Install Tools
        apt-get install sudo git awk gcc cmake wget libedit-dev zlibc zlib1g zlib1g-dev libopenblas-dev doxygen llvm-3.9-dev -y
else
        echo "APT Exists Using Over APT-GET"
        apt update
        wget -O - http://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -
        sudo apt-add-repository "deb http://apt.llvm.org/xenial/ llvm-toolchain-xenial-3.9 main"
        apt install git sudo awk gcc cmake wget libedit-dev zlibc zlib1g zlib1g-dev libopenblas-dev doxygen -y
fi

echo "Tools From Linux Repo Installed"
echo "Now Downloading External Tools"

# Download And Build SWIG
wget http://prdownloads.sourceforge.net/swig/swig-3.0.12.tar.gz
tar zxvf swig-3.0.12.tar.gz && cd swig-3.0.12
echo "Building Tools In 5 Seconds"
# Sleep 5 Seconds To Allow User To Read Message Above :)
sleep 5s
./configure --without-pcre && make && sudo make install
cd ..

# Clone ELL
echo "Downloadng ELL With GIT"
git clone https://github.com/Microsoft/ELL.git
mkdir build
cd build
echo "Now Building ELL"
cmake ..
make
make _ELL_python
read -p "Would You Like To Build The Documentation ? [Y/N] " -n 1 -r
if [[ $REPLY =~ ^[Yy]$ ]]
then
        echo ""
        echo "$GREEN OK - Building Documentation $DEFAULT"
        make doc
else
        echo ""
        echo "OK - Not Building Documentation"
        echo "Build Complete"
fi
