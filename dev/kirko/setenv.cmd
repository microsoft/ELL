@echo off
title EMLL
doskey self=cd /d %cd%\$*
doskey root=cd /d %cd%\..\..\$*
doskey py=cd /d %cd%\..\..\interfaces\python\test\$*
doskey libs=cd /d %cd%\..\..\libraries\$*
doskey list=vim -R $*
set PATH=%PATH%;c:\Program Files (x86)\MSBuild\14.0\Bin\
