pushd C:\src\llvm\build\Debug\bin
llvm-as.exe %1 -o %~p1%~n1.bc
popd