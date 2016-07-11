mkdir .\dist\win\build\Release

::copy /Y package.json .\dist
::copy /Y index_multiplat.js .\dist\index.js
::copy /Y emll.d.ts .\dist
::copy /Y swig\EMLL_wrap.xml .\dist
copy /Y build\Release\emll.node .\dist\win\build\Release

git -C ../EMLL ref-parse --short HEAD > ./dist/osx/EMLL_revision.txt
