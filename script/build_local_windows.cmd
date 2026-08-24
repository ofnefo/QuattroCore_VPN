@echo off
setlocal

set "QUATTRO_ROOT=%~dp0.."
set "VS_VCVARS=%ProgramFiles(x86)%\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

if not exist "%VS_VCVARS%" (
  echo Visual Studio 2022 Build Tools were not found.
  exit /b 1
)

call "%VS_VCVARS%"
if errorlevel 1 exit /b %errorlevel%

"%ProgramFiles%\CMake\bin\cmake.exe" ^
  -S "%QUATTRO_ROOT%" ^
  -B "%QUATTRO_ROOT%\build" ^
  -GNinja ^
  -DCMAKE_MAKE_PROGRAM="%QUATTRO_ROOT%\tools\ninja.exe" ^
  -DCMAKE_BUILD_TYPE=RelWithDebInfo ^
  -DCMAKE_PREFIX_PATH="%QUATTRO_ROOT%\tools\Qt\lib\cmake" ^
  -DOPENSSL_ROOT_DIR="%QUATTRO_ROOT%\tools\openssl" ^
  -DINPUT_VERSION=0.1.0-dev ^
  -DNKR_PACKAGE=1
if errorlevel 1 exit /b %errorlevel%

"%ProgramFiles%\CMake\bin\cmake.exe" --build "%QUATTRO_ROOT%\build" --parallel
exit /b %errorlevel%
