pushd C:\src\llvm\build\Debug\bin
@rem llc.exe -filetype=obj %1 
llc.exe -march=arm -filetype=obj %1 
popd