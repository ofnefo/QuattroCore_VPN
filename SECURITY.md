# Security policy

Quattro controls system routing, DNS, proxy settings and a privileged core. Security reports should not be posted as public issues.

After the repository is created, use GitHub's private vulnerability reporting for `ofnefo/quattro-desktop`. Until then, keep reports local and do not include credentials in screenshots or logs.

## Never commit

- subscription URLs or provider tokens
- GitHub, cloud or signing credentials
- private keys and client certificates
- `quattro.db`, `quattro_stats.db`, logs, dumps or generated configs
- release-signing material

## Update guarantees

Official application updates must originate from `ofnefo/quattro-desktop` releases. Release artifacts should be built by the repository workflow, accompanied by checksums and signed when signing infrastructure becomes available.
