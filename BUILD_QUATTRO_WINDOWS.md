# Building Quattro on Windows

## Required tools

- Visual Studio 2022 Build Tools with MSVC C++ and a Windows SDK
- CMake 3.20 or newer
- Ninja
- Qt 6.11.1 matching the target architecture
- OpenSSL matching the target architecture
- Go 1.26.5 and `protoc` when rebuilding `QuattroCore` and `QuattroUpdater`

## Build the Go components

The release workflow calls `script/build_go.sh`. It builds both components from this repository:

- `QuattroCore.exe`
- `QuattroUpdater.exe`

No prebuilt updater is downloaded from the upstream project.

## Build the Qt application

From an x64 Native Tools shell:

```powershell
cmake -S . -B build -GNinja `
  -DCMAKE_BUILD_TYPE=RelWithDebInfo `
  -DCMAKE_PREFIX_PATH="$PWD/tools/Qt/lib/cmake" `
  -DOPENSSL_ROOT_DIR="$PWD/tools/openssl" `
  -DINPUT_VERSION=0.1.0-dev
cmake --build build --parallel
```

Place `QuattroCore.exe`, `QuattroUpdater.exe` and, for supported modern Windows builds, `libcronet.dll` beside `Quattro.exe` before running the package.

Do not rename these executables: the privileged core verifies that its parent is `Quattro.exe`, and the application expects the core/updater names above.

## Package

`script/windows_installer.nsi` creates `QuattroSetup.exe`, Quattro shortcuts and an isolated `%APPDATA%\Quattro` configuration location.
