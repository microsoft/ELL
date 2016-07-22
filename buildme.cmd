:: Build the EMLL module

call node-gyp rebuild

:: TODO: see if this step is really necessary. It isn't on OS X
call node-gyp rebuild --target=0.36.7 --arch-x64 --dist-url=https://atom.io/download/atom-shell

call copyWindowsLib
