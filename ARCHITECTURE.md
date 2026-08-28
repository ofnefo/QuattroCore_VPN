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
- direct service rules are data-driven domain/application entries; Steam keeps a
  convenience toggle while providers such as Yandex, Minimax and AnyDesk use the
  same editable rule model.
- desktop automatic selection excludes LTE/mobile-only and subscription
  decoration entries. Candidate tiers are diamond, fast/premium, then ordinary
  country servers; latency ranks candidates inside each tier.
- the manual picker mirrors the full ordered subscription; LTE endpoints remain
  selectable and decoration entries remain visible as non-connectable headings.
- automatic selection is sticky: a healthy active server is not rotated merely
  because another endpoint becomes slightly faster.

## Application updates

Quattro reads the GitHub Releases API at:

`https://api.github.com/repos/ofnefo/QuattroCore_VPN/releases`

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

The locally built `QuattroUpdate.exe` is a self-contained migration wrapper for
the current per-user Windows install. It carries the same top-level
`Quattro/` ZIP and runs `QuattroUpdater` from a temporary directory, allowing
the installed updater binary to be replaced safely without touching config.

## Branches

- `main` — reviewed, releasable source
- `develop` — integration branch
- feature branches — isolated changes merged into `develop`

The local clone keeps the original source remote as `upstream` and uses `origin` for `https://github.com/ofnefo/QuattroCore_VPN.git`.
