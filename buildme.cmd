IF EXIST build (rd /q /s build)
md build
cd build
cmake -DOXYGEN_ENABLED=OFF -G "Visual Studio 14 2015 Win64" ..
MSBuild EMLL.sln /t:Rebuild /p:Configuration=Release

