# Building the Embedded Learning Library (ELL) using CMake

Prerequisites
=============
If you don't have cmake installed, download and install it from <https://cmake.org/download/>

If you want to build the compiler part of ELL, you will need to have LLVM installed. On Windows, CMake will 
download a LLVM package via NuGet, but you must have a recent version (at least version 3.5) of NuGet installed in order
to download the package. You can find instructions for installing the NuGet CLI [here](https://docs.nuget.org/ndocs/guides/install-nuget).
If you have an older version of NuGet, you can update it in-place via the command `nuget.exe update -Self`.

External dependencies
---------------------
To install the external dependencies (OpenBLAS and LLVM) on Windows, use NuGet to restore the packages in the `external` directory.

    > nuget.exe restore external/packages.config -PackagesDirectory external

Python interfaces
-----------------
If you intend to build the python interfaces, you will need [Python 2.7](https://www.python.org/downloads/). 

You will also need [SWIG](http://www.swig.org/download.html) version 3.0 or above. On Windows, download the `swigwin.zip` file, 
unzip the directory and put it somewhere convenient (for example, `C:\swigwin`), and make sure 
to update your PATH environment variable to include this directory. 
On Ubuntu, note that `apt-get install swig` currently gets an outdated version of swig. On UNIX systems,
download the SWIG tarball, unpack it, and follow the instructions in swig's `INSTALL` file.  
  
Building ELL
=============
CMake can create a variety of build environments for different compilers and target platforms. 
Open a console (a.k.a. command prompt) and navigate to the main solution directory (the directory that this file, `INSTALL.md`, is in). 
Create a subdirectory named `build`, and change to that directory:

    > mkdir build
    > cd build

The next step depends on which build system you would like to use to build the library.

Building with makefiles on UNIX
-------------------------------
On unix systems, the easiest way to build ELL is to use cmake to create a makefile, and then run the makefile to build the libraries. Invoke cmake as follows:

    > cmake -G "Unix Makefiles" ..

**Important:** don't forget the two dots (..) at the end of the command! This command creates a file named `Makefile`, along with other files. 
Next, run the makefile to build the project: 

    > make

A directory named `bin` will appear inside `build`, with all of the executables in it.

Building with Visual Studio 2015 (Update 3) on Windows
------------------------------------------------------
If you don't have Visual Studio installed, you can download the free [Visual Studio Express for Desktop](http://www.microsoft.com/express/). 

The library currently can only be built in 64-bit mode on Windows. To create a 64-bit solution for Visual Studio 2015, invoke cmake as follows:

    > cmake -G "Visual Studio 14 2015 Win64" ..

**Important:** don't forget the two dots (..) at the end of the command! 

This command creates a solution file named `ELL.sln`, along with other files. 
The solution file can also build interfaces using SWIG (ON by default) and code documentation using Doxygen (OFF by default). You can change these options off by typing:

    > cmake -G "Visual Studio 14 2015 Win64" -DUSE_SWIG=false -DBUILD_DOCS=true ..

There are two ways to build the library. One option is to open the solution file in Visual Studio and build it there. To do this, type:

    > ELL.sln

Visual studio will open the solution. Chose either the "Debug" configuration or the "Release" configuration, and use the shortcut Ctrl+Shift+B to build the solution. 

The second option is to build directly in the command prompt using the visual studio command line compiler "MSBuild". Find the path to the MSBuild application, which should look something like this

    > "\Program Files (x86)\MSBuild\14.0\Bin\amd64\MSBuild.exe"

If you are able to run MSBuild, invoke the following command:

    > "\Program Files (x86)\MSBuild\14.0\Bin\amd64\MSBuild.exe" /m ELL.sln /p:Configuration=Release 

Both build methods create a directory named `bin` inside `build`, with all of executables in it.

Building with Visual Studio 2015 (Update 3) using Clang on Windows
------------------------------------------------------------------
Visual Studio 2015 supports compilation using the Clang compiler. To create the appropriate 64-bit Visual Studio solution, invoke cmake as follows:

    > cmake -G "Visual Studio 14 2015 Win64" -T v140_clang_c2 ..

As above, open Visual Studio by typing:
 
    > ELL.sln

