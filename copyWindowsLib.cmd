mkdir .\dist\win\build\Release

copy /Y build\Release\emll.node .\dist\win\build\Release

echo "EMLL revision: " > .\dist\win\EMLL_revision.txt
git -C ../EMLL rev-parse HEAD > .\dist\win\EMLL_revision.txt
