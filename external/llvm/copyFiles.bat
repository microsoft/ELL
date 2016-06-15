@echo off
setlocal
set src=\src\llvm
set options=/y /d

xcopy %options% %src%\build\Debug\lib\LLVMCore.lib lib\
xcopy %options% %src%\build\Debug\lib\LLVMAnalysis.lib lib\
xcopy %options% %src%\build\Debug\lib\LLVMAsmParser.lib lib\
xcopy %options% %src%\build\Debug\lib\LLVMCore.lib lib\
xcopy %options% %src%\build\Debug\lib\LLVMSupport.lib lib\
xcopy %options% %src%\build\Debug\lib\LLVMBitWriter.lib lib\

set options=/y /d /s

xcopy %options% %src%\include\* include\
xcopy %options% %src%\build\include\llvm\Support\*.h include\llvm\Support\
xcopy %options% %src%\build\include\llvm\Config\*.h include\llvm\Config\
xcopy %options% %src%\build\include\llvm\IR\*.inc include\llvm\IR\
xcopy %options% %src%\build\include\llvm\IR\*.gen include\llvm\IR\

:Done
endlocal
exit /b %ERRORLEVEL%
