# Quattro Desktop release checklist

This checklist is the acceptance gate for a Windows build tailored to a Quattro
subscription. Run it on a clean Windows user profile before publishing a release.

## First launch and subscription

- Quattro opens on the simple dashboard; the advanced interface is one click away.
- With no subscription, the server selector and Connect button are disabled.
- A valid HTTPS subscription can be pasted and submitted with Enter or **Add**.
- The dashboard shows loading, success, empty-result, and error guidance without
  exposing the subscription token after entry.
- **Update** refreshes the active Quattro group and the server selector in place.
- Periodic subscription refresh is enabled after the first successful import.

## Connection and server choice

- TUN and System Proxy are mutually exclusive and persist independently.
- Manual server selection switches immediately when already connected.
- **Auto** selects healthy servers by subscription tier: diamond first,
  fast/premium second, then ordinary country servers. Latency decides within a
  tier, and a healthy active member is kept to avoid routine server hopping.
- LTE/mobile-only endpoints and decorative subscription rows never enter the
  desktop Auto pool. The manual picker contains every subscription row, starts
  with the diamond `Авто | Самый быстрый` endpoint, then the remaining Auto
  endpoints, and preserves the relative order of everything else. LTE endpoints
  are selectable and decoration rows are visible but disabled.
- Connect is disabled while connecting/disconnecting, preventing duplicate starts.
- The dashboard and tray display the real active member when Auto is running.

## Routing

- Russia Bypass sends Russian destinations directly and keeps protected services
  on the VPN path.
- Russia Bypass is active on a new/untouched configuration and sends Russian
  destinations directly without erasing Quattro service rules.
- Google/Gemini can use a separately selected subscription server.
- Steam remains a dedicated direct-routing toggle.
- The service table can add, edit and remove domain and application rules;
  Yandex/Yandex Disk, VK/Mail.ru, Ozon/Wildberries and 2GIS defaults persist
  when the dialog is reopened. Minimax and AnyDesk are not added automatically.
- ChatGPT/OpenAI, Microsoft/Copilot, Telegram, and Adobe use the main VPN path.

## Tray and startup

- Tray icon uses the Quattro mark and distinct off/TUN/System Proxy indicators.
- Tray menu exposes Open, Connect/Disconnect, server choice, routing, mode,
  Windows startup, restore connection, restart, and exit.
- Single-clicking the tray icon shows/hides the Quattro dashboard.
- Unified dashboard autostart creates a Windows task with `-tray`, remembers the
  selected server and mode, and reconnects without showing the main window.
- Disabling unified autostart removes the task and stops automatic reconnect.

## Packaging and isolation

- Installer creates `Quattro.lnk` on Desktop and Start Menu using `Quattro.exe`.
- Installer text renders English and Russian characters correctly on Windows.
- Quattro uses its own AppData directory, database, URL scheme, scheduled task,
  core socket, updater, and process names.
- Installing or uninstalling Quattro does not stop, overwrite, or delete Throne.
- The packaged build contains `Quattro.exe`, `QuattroCore.exe`,
  `QuattroUpdater.exe`, required runtime files, and valid SHA-256 release sidecars.
- A packaged AppData installation offers in-app update when `QuattroUpdater.exe`
  is present, and the updater preserves the local configuration directory.
