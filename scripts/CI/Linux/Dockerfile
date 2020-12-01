#--------------------------------------------------------------------------
# Quickstart docker container for ELL (https://microsoft.github.io/ELL/)
# Ubuntu 18.04, Miniconda, Python 3.6
#--------------------------------------------------------------------------

FROM continuumio/miniconda3:latest

RUN apt-get update \
    && apt-get install -y \
      build-essential \
      curl \
      gcc-8 \
      g++-8 \
      git \
      git-lfs \
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

# LLVM 8.0
RUN echo deb http://apt.llvm.org/bionic/ llvm-toolchain-bionic-8 main >> /etc/apt/sources.list
RUN echo deb-src http://apt.llvm.org/bionic/ llvm-toolchain-bionic-8 main >> /etc/apt/sources.list
RUN sudo wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -
RUN apt-get update \
    && apt-get install -y \
      llvm-8 \
      clang-8 \
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

# LD path to libpython3.6m.so
RUN echo /opt/conda/lib >> /etc/ld.so.conf.d/conda.conf && \
    ldconfig

# cmake
RUN curl -o cmake-3.15.2-Linux-x86_64.sh -L https://cmake.org/files/v3.15/cmake-3.15.2-Linux-x86_64.sh \
    && chmod +x cmake-3.15.2-Linux-x86_64.sh \
    && ./cmake-3.15.2-Linux-x86_64.sh --skip-license \
    &&  ln -s /opt/cmake-3.15.2-Linux-x86_64/bin/* /usr/local/bin

# Install pyaudio for ELL audio tutorials
RUN sudo apt-get install -y portaudio19-dev
RUN sudo apt-get install -y python3-pyaudio

# setup conda environment.
RUN conda create -n py37 numpy python=3.7
RUN /bin/bash -c ". activate py37 && pip install configparser opencv-python dask flake8 python-dateutil librosa matplotlib numpy onnx paramiko parse psutil pyaudio python_speech_features requests serial"

RUN sudo apt-get autoremove \
    && sudo apt-get clean all

# Jekyll for building website
RUN sudo apt-get -y install ruby ruby-dev zlib1g-dev 
RUN sudo gem install jekyll bundler github-pages jekyll-paginate jekyll-sass-converter kramdown rouge jekyll-seo-tag jekyll-relative-links

