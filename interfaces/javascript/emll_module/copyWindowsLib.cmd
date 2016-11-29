mkdir .\dist\win\build\Release

copy /Y build\Release\emll.node .\dist\win\build\Release

echo "EMLL revision: " > .\dist\win\EMLL_revision.txt
git rev-parse HEAD > .\dist\win\EMLL_revision.txt
