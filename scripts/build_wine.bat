@echo off

set mypath=%~dp0

set QTPATH=c:\work\Tools\Qt\6.2.4\msvc2019_64\bin
set QTDIR=c:\work\Tools\Qt\6.2.4\msvc2019_64

rem set QTPATH=c:\Qt\6.2.4\msvc2019_64\bin
rem set QTDIR=c:\Qt\6.2.4\msvc2019_64

set INIT_MSVC_ENV=c:\work\Tools\msvc\x64-msvc2019.bat
rem set INIT_MSVC_ENV="C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

call %INIT_MSVC_ENV%

set path=C:\work\tools\CMake\3.23.2\bin;C:\work\Tools\NSIS2;C:\work\Tools\Ninja\1.10.2;%path%


rem BUILDING Easy Budget
echo "Building Easy Budget" (%time%)
set build_path=%mypath%\win_build
set src_path=%mypath%\..
if not exist %build_path% mkdir %build_path% 

cd  %build_path%
cmake  -DCMAKE_TRY_COMPILE_CONFIGURATION=Release -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="%QTDIR%" -G"Ninja" %src_path%
cmake --build . 

rem Generate packaging
cpack --config "%mypath%\..\setup\cpack_config.cmake" -D CPACK_PROJECT_CONFIG_FILE="%mypath%\..\setup\cpack_generator.cmake" -G NSIS .

cd %mypath%

pause