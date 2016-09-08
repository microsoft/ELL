@echo off
title EMLL
doskey self=cd /d %cd% $*
doskey root=cd /d %cd%\..\..\ $*
doskey py=cd /d %cd%\..\..\Examples\python\ $*
doskey list=vim -R $*
set PATH=%PATH%;c:\Program Files (x86)\MSBuild\14.0\Bin\
