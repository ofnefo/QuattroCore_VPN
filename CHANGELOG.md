# Changelog

All notable Quattro changes will be documented here.

## [Unreleased]

## [0.1.2] - 2026-08-28

### Changed

- Removed Minimax and AnyDesk from direct-routing defaults and automatically
  retires their old Quattro-managed rules after an update.
- Seeded the editable direct list with Russian services: Yandex/Yandex Disk,
  VK/Mail.ru, Ozon/Wildberries and 2GIS; Steam remains a separate toggle.
- Removed the Windows-specific smart-routing subtitle from the dashboard.
- Made the manual picker match the regular subscription view: diamond
  `Авто | Самый быстрый` first, the remaining Auto endpoints next, and every
  other subscription row in its original relative order.

## [0.1.1] - 2026-08-28

### Changed

- Replaced the combined server drop-down with an explicit Auto button and a
  separate manual server picker that mirrors the full subscription, including
  LTE endpoints and non-connectable section headings.
- Prioritized desktop Auto selection as diamond, fast/premium, then ordinary
  country servers; LTE/mobile-only endpoints and subscription separators are
  excluded from the desktop pool.
- Replaced the Minimax-specific switch with an editable direct-routing table
  seeded with Yandex, Minimax and AnyDesk, while keeping Steam as a quick toggle.
- Enabled the built-in Russia Bypass for untouched default configurations.
- Built the Windows installer as Unicode so Russian text renders correctly.
- Restored packaged AppData self-updates through the bundled Quattro updater.
- Added a self-contained `QuattroUpdate.exe` for one-click migration of an
  existing per-user installation while preserving profiles and settings.
- Made localized Windows/Ninja builds reproducible by preserving MSVC's UTF-8
  dependency output and fetching the routing catalogue when absent.
- Established the independent Quattro application, core, updater, IPC, URL-scheme and data identities.
- Configured `ofnefo/QuattroCore_VPN` as the application update channel.
- Added the Quattro dashboard, selective-routing controls and sticky failover behavior.
- Replaced the external updater binary with a repository-built `QuattroUpdater`.

### Security

- Kept subscription credentials out of source defaults and documented secret-handling rules.
