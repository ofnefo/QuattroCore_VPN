# Quattro 0.1.1

First supported Quattro desktop release.

## Highlights

- Added an explicit **Auto** server button and a separate manual picker that
  mirrors the complete subscription order, including manually selectable LTE
  endpoints and visible section headings.
- Desktop Auto now prioritizes diamond servers, then fast/premium servers,
  then ordinary country servers. LTE/mobile-only endpoints are never selected
  automatically.
- Enabled Russia Bypass for untouched default configurations using
  `geosite-ru` and `geoip-ru`.
- Added an editable direct-routing table prefilled for Yandex/Yandex Disk,
  Minimax and AnyDesk. Steam remains a separate quick switch.
- Fixed Russian and English text rendering in the Unicode Windows installer.
- Moved the update channel to `ofnefo/QuattroCore_VPN` and restored atomic
  packaged updates with checksum verification.
- Added a standalone Windows `QuattroUpdate` package for updating an existing
  `%LOCALAPPDATA%\Quattro` installation without deleting subscriptions,
  profiles or settings.

## Windows files

- `Quattro-v0.1.1-windows-amd64-installer.exe` — normal installation on a
  64-bit Intel/AMD Windows PC.
- `Quattro-v0.1.1-windows-update.exe` — one-click update for an existing
  per-user installation.
- `Quattro-v0.1.1-windows-amd64.zip` — application update payload consumed by
  the built-in updater.
- Every ZIP and EXE is accompanied by a `.sha256` checksum file.

The GitHub release workflow additionally produces the true universal Windows
installer after its x64, ARM64 and legacy build matrix has completed.

The Windows binaries are currently unsigned. Windows SmartScreen may show a
warning until a code-signing certificate is configured.
