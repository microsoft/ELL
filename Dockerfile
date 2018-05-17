# Dockerfile for an Ubuntu environment for ELL

FROM continuumio/miniconda3:latest

RUN apt-get update \
    && apt-get install -y \
      build-essential \
      cmake \
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
RUN apt-get update \
    && apt-get install -y \
      llvm-3.9-dev \
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
    && ./configure --prefix=/usr/local/mpi

WORKDIR /openmpi-1.10.3
RUN make -j all \
    && make install
WORKDIR /

# LD path to libpython3.6m.so
RUN echo /opt/conda/lib >> /etc/ld.so.conf.d/conda.conf && \
    ldconfig

# ELL
RUN git clone https://github.com/Microsoft/ELL.git \
    && cd ELL \
    && mkdir -p build \
    && cd build \
    && cmake .. \
    && make \
    && make _ELL_python
