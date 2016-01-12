How to build the Bonsai project using cmake:

If you don't currently have cmake installed, install it from:

   https://cmake.org/download/


In the main solution directory (the directory this file is in), create
a subdirectory called build, and change to that directory.

  > mkdir build
  > cd build


UNIX:
Now invoke cmake to create a build environment (in this case, using makefiles):

    > cmake -G "Unix Makefiles" ..

Now build the project(s) using make:

    > make

You can build individual projects if you like:

    > make print


WINDOWS:
If you'd like to create a Visual Studio solution instead of Makefiles,
just change the cmake command to look like this:

    > cmake -G "Visual Studio 14 2015" ..

...which will create a solution targeting x86. You can specify 64-bit
mode with the following:

    > cmake -G "Visual Studio 14 2015 Win64" ..

I'm sure there's some configuration you can add in the CMakeLists.txt
files to add 64-bit support without having to explicitly type it out
on the command line, but I don't know what it is.


Note:
Often cmake will become confused if you try to invoke it again with
different settings. You can usually just remove the build/CMakeCache.txt file
and re-run cmake. I generally "rm -rf *" everything in the build
directory though, just to be sure.

