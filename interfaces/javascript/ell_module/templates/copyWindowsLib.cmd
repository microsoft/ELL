set DIST=.\dist\win\build\Release

if not exist %DIST% mkdir %DIST%

copy /Y build\Release\ell.node %DIST%

echo "ELL revision: " > .\dist\win\ELL_revision.txt
git rev-parse HEAD > .\dist\win\ELL_revision.txt
