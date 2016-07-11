mkdir -p ./dist/osx/build/Release

cp -f build/Release/emll.node ./dist/osx/build/Release

git -C ../EMLL rev-parse --short HEAD > ./dist/osx/EMLL_revision.txt
