@echo off
pushd C:\src\llvm\build\Debug\bin
@rem llc.exe -march=thumb -mcpu=cortex-m0 -mattr=+soft-float -filetype=asm -asm-verbose=0 %1 
llc.exe -march=thumbeb -mcpu=cortex-m0 -float-abi=soft -mattr=+soft-float,+armv6 -meabi=gnu -filetype=asm -asm-verbose=0 %1 
popd 