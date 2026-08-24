# Changelog

All notable Quattro changes will be documented here.

## [Unreleased]

### Changed

- Established the independent Quattro application, core, updater, IPC, URL-scheme and data identities.
- Reserved `ofnefo/quattro-desktop` as the application update channel.
- Added the Quattro dashboard, selective-routing controls and sticky failover behavior.
- Replaced the external updater binary with a repository-built `QuattroUpdater`.

### Security

- Kept subscription credentials out of source defaults and documented secret-handling rules.
