:: Build the ELL module
@echo off
 
Echo "%1"
call npm install

IF "%1"=="" (
  Echo building ELL for Electron
  REM call node-gyp rebuild
  
   call node-gyp rebuild --runtime=electron --target=1.7.9 --arch-x64 --dist-url=https://atom.io/download/atom-shell
)

IF /I "%1"=="node" (
  Echo building ELL for Node.JS
  call node-gyp rebuild
)
