mkdir -p ./dist/linux/build/Release

cp -f build/Release/emll.node ./dist/linux/build/Release

cp -f build/EMLLXML_wrap.xml ./dist/EMLLXML_wrap.xml

echo "EMLL revision: " > ./dist/linux/EMLL_revision.txt
git -C ../EMLL rev-parse HEAD >> ./dist/linux/EMLL_revision.txt
