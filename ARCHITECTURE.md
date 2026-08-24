# Quattro architecture

## Components

### Quattro GUI

The Qt application owns configuration, subscriptions, routing policy, tray controls and platform integration. Its executable identity is `Quattro`.

### QuattroCore

The Go core owns TUN, DNS, proxy protocols and privileged network operations. It accepts IPC only from a parent executable named `Quattro` in the same directory. The GUI supplies the socket through `QUATTRO_CORE_SOCKET`.

### QuattroUpdater

The updater is built from `core/server/cmd/quattro-updater`. It accepts a `Quattro.zip` archive containing one top-level `Quattro/` directory, rejects archive traversal, preserves the local `config/` directory, replaces application files and restarts Quattro.

## Local state

Packaged builds use the platform application-data directory for Quattro. The primary databases are `quattro.db` and `quattro_stats.db`. This deliberate separation prevents accidental modification of another client's configuration.

Subscription credentials are local state. They must never be used as compiled defaults, examples, test fixtures or CI secrets.

## Routing model

- `direct` handles Russian/local destinations and explicitly direct services.
- `proxy` handles selected blocked/international services.
- service channels may use a dedicated outbound group for region-sensitive providers.
- automatic selection is sticky: latency ranks candidates, but a healthy active server is not rotated merely because another endpoint becomes slightly faster.

## Application updates

Quattro reads the GitHub Releases API at:

`https://api.github.com/repos/ofnefo/quattro-desktop/releases`

Supported release names use semantic versions such as `v0.1.0`, `v0.2.0-beta.1` or `v1.0.0-rc.2`. ZIP assets must use these names:

- `Quattro-v0.1.0-windows-amd64.zip`
- `Quattro-v0.1.0-windows-arm64.zip`
- `Quattro-v0.1.0-windowslegacy-amd64.zip`
- `Quattro-v0.1.0-windowslegacy-386.zip`
- `Quattro-v0.1.0-linux-amd64.zip`
- `Quattro-v0.1.0-linux-arm64.zip`
- `Quattro-v0.1.0-macos-amd64.zip`
- `Quattro-v0.1.0-macos-arm64.zip`

Each ZIP has a matching `.sha256` release asset. The GUI downloads the selected archive as `Quattro.zip`, verifies SHA-256, shuts down networking cleanly and launches `QuattroUpdater`. Subscription refresh is independent of this release channel.

## Branches

- `main` — reviewed, releasable source
- `develop` — integration branch
- feature branches — isolated changes merged into `develop`

The local clone keeps the original source remote as `upstream` and reserves `origin` for `https://github.com/ofnefo/quattro-desktop.git`.
