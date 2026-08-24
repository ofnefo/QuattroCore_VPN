# Quattro Desktop

Quattro is a cross-platform desktop VPN and proxy client focused on simple selective routing: blocked and selected international services use a tunnel, while Russian and ordinary direct traffic can bypass it.

> Development status: the independent Quattro fork is being prepared. No public repository or supported release has been published yet.

## Product goals

- One-link subscription setup with scheduled refresh.
- TUN and system-proxy modes.
- Russia Bypass routing that can be enabled or disabled.
- Sticky lowest-latency selection: keep the current server while it is healthy and fail over immediately when it stops responding.
- Separate channels for services that need a specific region, such as Google/Gemini.
- Direct routing for services that do not need a tunnel, including Steam and optionally Minimax.
- A compact dashboard, tray controls, autostart and reconnect-last-profile.

## Independent application identity

The fork uses its own external and internal contracts:

- GUI: `Quattro` / `Quattro.exe`
- privileged core: `QuattroCore` / `QuattroCore.exe`
- updater: `QuattroUpdater` / `QuattroUpdater.exe`
- URL scheme: `quattro://`
- IPC environment: `QUATTRO_CORE_SOCKET` and `QUATTRO_CORE_DEBUG`
- data directory: the platform-specific Quattro application-data directory
- databases: `quattro.db` and `quattro_stats.db`

Quattro does not reuse or modify an installed copy of the upstream application.

## Update channel

The application is configured to read releases from the future repository:

- repository: `https://github.com/ofnefo/quattro-desktop`
- API: `https://api.github.com/repos/ofnefo/quattro-desktop/releases`

The repository does not exist yet. Until it is created and a release is published, the update check will safely return no usable update.

Release assets must follow the contract documented in [ARCHITECTURE.md](ARCHITECTURE.md). Application updates and subscription/server-list updates are separate systems.

## Source layout

- `src/`, `include/` — Qt desktop application
- `core/server/` — Go core and the Quattro updater command
- `res/` — Quattro resources, translations and platform icons
- `script/` — packaging and deployment scripts
- `.github/workflows/` — build and release automation

## Building

Windows build notes are in [BUILD_QUATTRO_WINDOWS.md](BUILD_QUATTRO_WINDOWS.md). The complete release matrix is defined in [.github/workflows/build.yml](.github/workflows/build.yml).

## Security

Never commit subscription URLs, access tokens, private keys, generated configurations or user databases. See [SECURITY.md](SECURITY.md).

## License and origin

Quattro is a GPL-3.0 fork derived from the open-source [Throne](https://github.com/throneproj/Throne) project. The GPL license is retained in [LICENSE](LICENSE), and origin/attribution details are recorded in [QUATTRO_NOTICE.md](QUATTRO_NOTICE.md). Upstream names that remain in dependency module URLs or legal notices are attribution and dependency identifiers, not Quattro application branding.
