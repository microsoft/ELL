#!/bin/bash
export PATH="/home/pi/miniconda3/bin:$PATH"
source activate py34
cd /home/pi/pi3
if [ -d "build" ]; then
    rm -rf build
fi
mkdir build && cd build
cmake ..
make
cd ..
export DISPLAY=:0
python demo.py darknet_config.json darknetImageNetLabels.txt schoolbus.png -iterations 1
