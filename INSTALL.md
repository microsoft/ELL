Building Rockmill using cmake
-----------------------------
If you don't have cmake installed, download and install it from:

  <https://cmake.org/download/>

Open a console (a.k.a. command prompt) and navigate to the main solution directory (the directory that this file is in). Create a subdirectory named "build", and change to that directory:

    > mkdir build
    > cd build

Building with makefiles on UNIX
-------------------------------
On unix systems, the easiest way to build Rockmill is to get cmake to create a makefile, and then run the makefile to build Rockmill. Invoke cmake as follows:

    > cmake -G "Unix Makefiles" ..

Important: don't forget the two dots (..) at the end of the command! This command creates a file named "Makefile" and a bunch of other stuff. Now run the makefile to build the project: 

    > make

Building with Visual Studio 2015 on Windows
-------------------------------------------
Make sure you have Visual Studio installed. You can download the free "Visual Studio Express for Desktop" from:

   http://www.microsoft.com/express/ 

To create a 32-bit solution for Visual Studio 2015, invoke cmake as follows:

    > cmake -G "Visual Studio 14 2015" ..

Important: don't forget the two dots (..) at the end of the command! This command creates a solution file named "Rockmill.sln" and a bunch of other stuff. There are two ways to build the library. The first option is to open the solution file in Visual Studio and build it there. To do this, simply type:

    > Rockmill.sln

Visual studio should open the solution. Chose either the "Debug" configuration or the "Release" configuration, and use the shortcut Ctrl+Shift+B to build the solution. 

The second option is to build directly in the command prompt using the visual studio command line compiler "MSBuild". Find the path to the MSBuild application, which should look something like this

    > "\Program Files (x86)\MSBuild\14.0\Bin\amd64\MSBuild.exe"

If you are able to run MSBuild, invoke the following command to build Rockmill

    > "\Program Files (x86)\MSBuild\14.0\Bin\amd64\MSBuild.exe" /m Rockmill.sln /p:Configuration=Release 
 
As an alternative to the above, you may want to create a 64-bit solution for Visual Studio 2015. To do this, invoke cmake as follows:

    > cmake -G "Visual Studio 14 2015 Win64" ..

Repeat the build process described above.
