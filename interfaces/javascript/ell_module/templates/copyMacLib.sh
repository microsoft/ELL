#!/bin/bash

mkdir -p ./dist/mac/build/Release

cp -f build/Release/ell.node ./dist/mac/build/Release

echo "ELL revision: " > ./dist/mac/ELL_revision.txt
git rev-parse HEAD >> ./dist/mac/ELL_revision.txt
