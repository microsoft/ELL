How to build project using cmake:

In the main solution directory (the directory this file is in), create
a subdirectory called build, and change to that directory.

  > mkdir build
  > cd build

Now invoke cmake to create a build environment (in this case, using makefiles):

    > cmake -g "Unix Makefiles" ..

Now build the project(s) using make:

    > make

You can build individual projects if you like:

    > make print

