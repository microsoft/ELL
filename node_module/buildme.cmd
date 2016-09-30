:: Build the EMLL module
@echo off
 
Echo "%1"
IF "%1"=="" (
  Echo building EMLL for Electron
  call node-gyp rebuild
  
  :: TODO: see if this step is really necessary. It isn't on OS X
  call node-gyp rebuild --target=0.36.7 --arch-x64 --dist-url=https://atom.io/download/atom-shell
)

IF /I "%1"=="node" (
  Echo building EMLL for Node.JS
  call node-gyp rebuild
)

:: call copyWindowsLib
