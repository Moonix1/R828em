@ECHO OFF
PUSHD %~dp0\..\
CALL .\libs\premake\premake5.exe vs2022
POPD
PAUSE