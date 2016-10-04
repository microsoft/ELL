mkdir -p ./dist/osx/build/Release

cp -f build/Release/emll.node ./dist/osx/build/Release

cp -f build/EMLLXML_wrap.xml ./dist/EMLLXML_wrap.xml

echo "EMLL revision: " > ./dist/osx/EMLL_revision.txt
git rev-parse HEAD >> ./dist/osx/EMLL_revision.txt
