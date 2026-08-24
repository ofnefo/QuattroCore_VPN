# Contributing to Quattro

## Workflow

1. Branch from `develop`.
2. Keep application, core and updater identity changes consistent across platforms.
3. Add or update tests for routing, update parsing and privileged IPC changes.
4. Run formatting and the relevant Qt/Go checks.
5. Open a pull request into `develop`; promote tested releases to `main`.

## Rules

- Do not commit subscription URLs, tokens, private keys, generated configs, databases or logs.
- Do not point the application updater at an upstream binary channel.
- Preserve `LICENSE`, `QUATTRO_NOTICE.md` and required third-party notices.
- Use `Quattro`, `QuattroCore`, `QuattroUpdater`, `quattro://` and `QUATTRO_*` for new product contracts.
- Treat TUN, DNS, system proxy and updater changes as high-risk; include a rollback description.

## Commit style

Use concise conventional prefixes where practical: `feat:`, `fix:`, `refactor:`, `docs:`, `build:` and `test:`.
