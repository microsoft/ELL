#!/bin/bash

mkdir -p ./dist/linux/build/Release

cp -f build/Release/ell.node ./dist/linux/build/Release

echo "ELL revision: " > ./dist/linux/ELL_revision.txt
git rev-parse HEAD >> ./dist/linux/ELL_revision.txt
