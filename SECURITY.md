# Security policy

Quattro controls system routing, DNS, proxy settings and a privileged core. Security reports should not be posted as public issues.

Use GitHub's private vulnerability reporting for `ofnefo/QuattroCore_VPN`. Do not include credentials in screenshots or logs.

## Never commit

- subscription URLs or provider tokens
- GitHub, cloud or signing credentials
- private keys and client certificates
- `quattro.db`, `quattro_stats.db`, logs, dumps or generated configs
- release-signing material

## Update guarantees

Official application updates must originate from `ofnefo/QuattroCore_VPN` releases. Release artifacts should be built by the repository workflow, accompanied by checksums and signed when signing infrastructure becomes available.
