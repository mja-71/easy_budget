@echo off

set mypath=%~dp0
set TOOLS_PATH=C:\Tools
set QTPATH=%TOOLS_PATH%\Qt\6.2.4\msvc2019_64\bin
set QTDIR=%TOOLS_PATH%\Qt\6.2.4\msvc2019_64

rem set QTPATH=c:\Qt\6.2.4\msvc2019_64\bin
rem set QTDIR=c:\Qt\6.2.4\msvc2019_64

set INIT_MSVC_ENV=%TOOLS_PATH%\msvc\x64-msvc2019.bat

call %INIT_MSVC_ENV%

set path=%TOOLS_PATH%\CMake\3.23.2\bin;%TOOLS_PATH%\NSIS;%TOOLS_PATH%\Ninja\1.10.2;%path%


rem BUILDING Easy Budget
echo "Building Easy Budget" (%date% %time%)
echo path is  %mypath%
set src_path=%mypath%\..

rem build in current path for write permission
set build_path=win_build
if not exist %build_path% mkdir %build_path% 

cd  %build_path%
cmake  -DCMAKE_TRY_COMPILE_CONFIGURATION=Release -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="%QTDIR%" -G"Ninja" %src_path%
cmake --build . 

rem Generate packaging
cpack --config "%mypath%\..\setup\cpack_config.cmake" -D CPACK_PROJECT_CONFIG_FILE="%mypath%\..\setup\cpack_generator.cmake" -G NSIS .

cd %mypath%

pause
