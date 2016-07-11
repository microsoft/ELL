mkdir -p ./dist/osx/build/Release

cp -f build/Release/emll.node ./dist/osx/build/Release

echo "EMLL revision: " > ./dist/osx/EMLL_revision.txt
git -C ../EMLL rev-parse HEAD >> ./dist/osx/EMLL_revision.txt
