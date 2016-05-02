Building the Embedded Machine Learning Libraries (EMLL) using CMake
===================================================================

Prerequisites
-------------
If you don't have cmake installed, download and install it from:

   <https://cmake.org/download/>

To build the python interfaces, you will also need to have Python 2.7 and SWIG installed. Download Python 2.7 from the [download directory](https://www.python.org/downloads/) of <https://www.python.org/>. 

To install SWIG on Windows, download the swigwin zip file from <http://swig.org>'s [download section](http://www.swig.org/download.html). Unzip this directory
and put it somewhere convenient (for example, C:\swigwin). Make sure you set your PATH environment
variable to include the swigwin directory.

On other operating systems, download the swig tarball, unpack it, and follow the instructions in the `INSTALL` file.

Creating a build environment
----------------------------
CMake is a sort of meta-build system that takes a description of the files and other parameters needed to build a project and
creates a build environment for it using the native compiler and build system on the target platform. 

To create a build environment, first make a `build` directory that all of the generated files will live in.Open a console (a.k.a. command prompt) and navigate to 
the main solution directory (the directory that this file is in). Create a subdirectory named "build", and change to that directory:

    > mkdir build
    > cd build

What to do next depends on which build system you would like to use to build the library.

Building with makefiles on UNIX
-------------------------------
On unix systems, the easiest way to build EMLL is to get cmake to create a makefile, and then run the makefile to build the libraries. Invoke cmake as follows:

    > cmake -G "Unix Makefiles" ..

Important: don't forget the two dots (..) at the end of the command! This command creates a file named "Makefile" and a bunch of other stuff. Now run the makefile to build the project: 

    > make

A directory named "bin" should appear inside "build", with all of the executables in it.

Building with Visual Studio 2015 on Windows
-------------------------------------------
Make sure you have Visual Studio installed. You can download the free "Visual Studio Express for Desktop" from:

   http://www.microsoft.com/express/ 

To create a 32-bit solution for Visual Studio 2015, invoke cmake as follows:

    > cmake -G "Visual Studio 14 2015" ..

Important: don't forget the two dots (..) at the end of the command! This command creates a solution file named "EMLL.sln" and a bunch of other stuff. There are two ways to build the library. The first option is to open the solution file in Visual Studio and build it there. To do this, simply type:

    > EMLL.sln

Visual studio should open the solution. Chose either the "Debug" configuration or the "Release" configuration, and use the shortcut Ctrl+Shift+B to build the solution. 

The second option is to build directly in the command prompt using the visual studio command line compiler "MSBuild". Find the path to the MSBuild application, which should look something like this

    > "\Program Files (x86)\MSBuild\14.0\Bin\amd64\MSBuild.exe"

If you are able to run MSBuild, invoke the following command to build EMLL

    > "\Program Files (x86)\MSBuild\14.0\Bin\amd64\MSBuild.exe" /m EMLL.sln /p:Configuration=Release 

Both build methods create a directory named "bin" inside "build", and put all of the executables in it. As an alternative to the above, you may want to create a 64-bit solution for Visual Studio 2015. To do this, invoke cmake as follows:

    > cmake -G "Visual Studio 14 2015 Win64" ..

Repeat the build process described above.
