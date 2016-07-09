:: Build the EMLL module

call node-gyp rebuild

:: TODO: see if this step is really necessary. It isn't on OS X
call node-gyp rebuild --target=0.36.7 --arch-x64 --dist-url=https://atom.io/download/atom-shell

::# when building for electron, do this (?) -- seems not to be necessary, above works now(?)
::# what's the HOME= part all about?
::# HOME=~/.electron-gyp node-gyp rebuild --target=0.36.7 --arch-x64 --dist-url=https://atom.io/download/atom-shell

:: TODO: copy the module from node_src/build/Release to somewhere sane (e.g., ../lib/emllModule/win)
