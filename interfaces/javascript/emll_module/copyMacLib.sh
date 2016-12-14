#!/bin/bash

mkdir -p ./dist/mac/build/Release

cp -f build/Release/emll.node ./dist/mac/build/Release

echo "EMLL revision: " > ./dist/mac/EMLL_revision.txt
git rev-parse HEAD >> ./dist/mac/EMLL_revision.txt
