pushd C:\src\llvm\build\Debug\bin
@rem llc.exe -march=cpp %1 -o %~p1%~n1.cpp
llc.exe %1 
popd