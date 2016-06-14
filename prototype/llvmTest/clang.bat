setlocal
set src=%~f1
set out=%~p1%~n1.ll
pushd C:\Program Files\LLVM\bin\
clang -S -emit-llvm %src% -o %out%
popd
endlocal