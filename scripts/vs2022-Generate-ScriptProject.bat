@echo off

pushd %~dp0\..\

cd Editor\Projects\First\Script

call ..\..\..\..\vendor\premake\premake5.exe vs2022

popd
PAUSE