# Dockerfile for an Ubuntu environment for ELL for Continuous Integration
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
    && apt-get clean all

# LLVM
RUN echo deb http://apt.llvm.org/bionic/ llvm-toolchain-bionic-8 main >> /etc/apt/sources.list
RUN echo deb-src http://apt.llvm.org/bionic/ llvm-toolchain-bionic-8 main >> /etc/apt/sources.list
RUN wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -
RUN apt-get update \
    && apt-get install -y \
      llvm-8 \
    && apt-get clean all

# SWIG
RUN curl -O --location http://prdownloads.sourceforge.net/swig/swig-3.0.12.tar.gz \
    && tar zxvf swig-3.0.12.tar.gz \
    && cd swig-3.0.12 \
    && ./configure --without-pcre \
    && make \
    && make install

# OpenCV
RUN apt-get update \
    && apt-get install -y \
       libgl1-mesa-glx
RUN export PATH="/opt/conda/bin:${PATH}" \
    && conda install --yes --quiet -c conda-forge opencv

# CNTK
RUN /bin/bash -c "source activate base" \
    && pip install --upgrade pip \
    && pip install --no-cache-dir --ignore-installed \
          cntk

# OpenMPI
RUN curl -o openmpi-1.10.3.tar.gz -L https://www.open-mpi.org/software/ompi/v1.10/downloads/openmpi-1.10.3.tar.gz \
    && tar zxvf openmpi-1.10.3.tar.gz \
    && cd openmpi-1.10.3 \
    && ./configure --prefix=/usr/local/mpi \
    && make -j all \
    && make install

# LD path to libpython3.6m.so
RUN echo /opt/conda/lib >> /etc/ld.so.conf.d/conda.conf && \
    ldconfig

# cmake
RUN curl -o cmake-3.11.2-Linux-x86_64.sh -L https://cmake.org/files/v3.11/cmake-3.11.2-Linux-x86_64.sh \
    && chmod +x cmake-3.11.2-Linux-x86_64.sh \
    && ./cmake-3.11.2-Linux-x86_64.sh --skip-license \
    &&  ln -s /opt/cmake-3.11.2-Linux-x86_64/bin/* /usr/local/bin
