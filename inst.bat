@echo off

msbuild WebWin.sln /clp:ErrorsOnly /m /p:Configuration="Debug" /p:Platform=x64
call clbcall

msbuild WebWin.sln /clp:ErrorsOnly /m /p:Configuration="Release" /p:Platform=x64
call clbcall
