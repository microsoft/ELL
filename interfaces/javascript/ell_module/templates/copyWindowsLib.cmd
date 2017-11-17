mkdir .\dist\win\build\Release

copy /Y build\Release\ell.node .\dist\win\build\Release

echo "ELL revision: " > .\dist\win\ELL_revision.txt
git rev-parse HEAD > .\dist\win\ELL_revision.txt
