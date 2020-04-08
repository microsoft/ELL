#--------------------------------------------------------------------------
# Quickstart docker container for ELL (https://microsoft.github.io/ELL/)
# Ubuntu 18.04, Miniconda, Python 3.6
#--------------------------------------------------------------------------

FROM continuumio/miniconda3:latest

RUN apt-get update \
    && apt-get install -y \
      build-essential \
      curl \
      gcc \
      git \
      libedit-dev \
      zlibc \
      zlib1g \
      zlib1g-dev \
      libopenblas-dev \
      doxygen \
      unzip \
      sudo \
      zip \
    && apt-get clean all

# LLVM
RUN echo deb http://apt.llvm.org/bionic/ llvm-toolchain-bionic-8 main >> /etc/apt/sources.list
RUN echo deb-src http://apt.llvm.org/bionic/ llvm-toolchain-bionic-8 main >> /etc/apt/sources.list
RUN sudo wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -
RUN apt-get update \
    && apt-get install -y \
      llvm-8 \
    && apt-get clean all

# LD paths to LLVM tools
RUN sudo ln -s /usr/lib/llvm-8/bin/opt /usr/bin/opt
RUN sudo ln -s /usr/lib/llvm-8/bin/llc /usr/bin/llc

# SWIG
RUN curl -O --location http://prdownloads.sourceforge.net/swig/swig-4.0.0.tar.gz \
    && tar zxvf swig-4.0.0.tar.gz \
    && cd swig-4.0.0 \
    && ./configure --without-pcre \
    && make \
    && make install \
    && cd .. \
    && rm swig-4.0.0.tar.gz \
    && rm -r -f swig-4.0.0 

# OpenCV
RUN apt-get update \
    && apt-get install -y \
       libgl1-mesa-glx
RUN export PATH="/opt/conda/bin:${PATH}" \
    && conda install --yes --quiet -c conda-forge opencv

# LD path to libpython3.6m.so
RUN echo /opt/conda/lib >> /etc/ld.so.conf.d/conda.conf && \
    ldconfig

# cmake
RUN curl -o cmake-3.15.2-Linux-x86_64.sh -L https://cmake.org/files/v3.15/cmake-3.15.2-Linux-x86_64.sh \
    && chmod +x cmake-3.15.2-Linux-x86_64.sh \
    && ./cmake-3.15.2-Linux-x86_64.sh --skip-license \
    &&  ln -s /opt/cmake-3.15.2-Linux-x86_64/bin/* /usr/local/bin

RUN sudo apt-get autoremove \
    && sudo apt-get clean all
