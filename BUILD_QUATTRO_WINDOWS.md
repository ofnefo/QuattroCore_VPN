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

With the dependencies unpacked under `tools/`, run:

```powershell
script\build_local_windows.cmd
```

The helper initializes the Visual Studio x64 environment, configures the packaged
AppData mode, and builds `build\Quattro.exe`. The equivalent manual commands from
an x64 Native Tools shell are:

```powershell
cmake -S . -B build -GNinja `
  -DCMAKE_BUILD_TYPE=RelWithDebInfo `
  -DCMAKE_PREFIX_PATH="$PWD/tools/Qt/lib/cmake" `
  -DOPENSSL_ROOT_DIR="$PWD/tools/openssl" `
  -DINPUT_VERSION=0.1.0-dev `
  -DNKR_PACKAGE=1
cmake --build build --parallel
```

Place `QuattroCore.exe`, `QuattroUpdater.exe` and, for supported modern Windows builds, `libcronet.dll` beside `Quattro.exe` before running the package.

Do not rename these executables: the privileged core verifies that its parent is `Quattro.exe`, and the application expects the core/updater names above.

## Package

`script/windows_installer.nsi` creates `QuattroSetup.exe`, Quattro shortcuts and an isolated `%APPDATA%\Quattro` configuration location.

For a local Windows x64 package after staging the four runtime files under
`deployment\windows-amd64`, run from the repository root:

```powershell
& "C:\Program Files (x86)\NSIS\makensis.exe" /NOCD /V3 `
  /DQUATTRO_LOCAL_X64 `
  /DAPP_VERSION=0.1.0-dev `
  /DAPP_VERSION_MAJOR=0 /DAPP_VERSION_MINOR=1 `
  /DAPP_VERSION_PATCH=0 /DAPP_VERSION_BUILD=0 `
  script\windows_installer.nsi
```

Without `QUATTRO_LOCAL_X64`, the release workflow keeps building the universal
installer and expects every architecture-specific deployment directory.
