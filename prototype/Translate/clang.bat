setlocal
set src=%~f1
set out=%~p1%~n1.ll
pushd C:\Program Files\LLVM\bin\
@rem clang -S -emit-llvm -O1 %src% -o %out%
clang -S -emit-llvm -fno-ms-compatibility %src% -o %out%
popd
endlocal