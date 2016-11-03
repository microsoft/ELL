mkdir -p ./dist/linux/build/Release

cp -f build/Release/emll.node ./dist/linux/build/Release

echo "EMLL revision: " > ./dist/linux/EMLL_revision.txt
git rev-parse HEAD >> ./dist/linux/EMLL_revision.txt
