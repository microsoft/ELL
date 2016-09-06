@echo off
title EMLL
doskey root=cd /d %cd% $*
doskey list=vim -R $*
set PATH=%PATH%;c:\Program Files (x86)\MSBuild\14.0\Bin\
