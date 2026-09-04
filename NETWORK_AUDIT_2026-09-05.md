> WITHDRAWN BUILD / INVALID RELEASE GATE: the locally packaged 0.1.2 changes
> caused a DNS startup regression and must not be installed. Core tests now
> reproduce the invalid external-Tailscale DNS detour. The server sweep ran
> while another Quattro TUN was active (confirmed by its log), so the 30/272
> result must not be interpreted as provider availability or server health.
> The user's GitHub v0.1.2 works. Follow-up work is on codex/0.1.3-dns-regression.

# Quattro: network audit, 5 September 2026

## Subscription and servers

The installed database's subscription timestamp was 24 August, with hourly
refresh enabled and no persisted latency measurements. This alone does not prove
the scheduler is broken: the inspected database may differ from the running
client's working directory.

The provider returned HTTP 200 and a sing-box JSON subscription with 272 real
server outbounds. All 261 TCP ports accepted a connection. Eleven outbounds use
Hysteria2 and were tested through the VPN core instead of a TCP-only check.

The core HTTPS sweep returned an HTTP response through 30 of 272 outbounds;
242 did not complete within the test conditions. This includes all four provider
Auto endpoints. Successful foreign examples included Germany #2, Switzerland and
Netherlands. Results are specific to this machine's network path, the time of
measurement and the HTTPS test target. A failed probe is not proof that a server
is permanently unavailable; an HTTP response is not a bandwidth measurement.

Full per-server measurements are local in `runtime-test/tunnel-audit.json`;
the TCP inventory is in `runtime-test/server-audit.json`. Neither file contains
subscription URLs, credentials or server addresses. These local reports are
excluded from Git.

## Changes

- Built-in bank, payment, government, Windows Update and Tailscale domain rules;
  direct local IPv4/IPv6 routes also apply to traffic received by the proxy.
- Shipped rules are refreshed in existing Russia channel profiles while user
  rules retain precedence. A customized Default profile is not auto-replaced.
- Direct DNS rules precede FakeIP fallback. LAN suffixes use the local resolver;
  an active external Tailscale interface enables Quad100 for `*.ts.net`.
- Recovery clears the Windows resolver cache and restarts the active VPN;
  transition buttons cannot launch another switch while connecting/disconnecting.
- Subscription reconciliation is serialized. Empty/invalid responses are rejected
  before clearing existing profiles. The dashboard receives an explicit success
  result rather than treating old nonempty profiles as proof of update success.
- Future last-run timestamps no longer suppress periodic work after clock changes.
- Daily application release checks notify about new versions without installing;
  the check can be disabled in basic settings.

## Validation boundaries

The standalone sweep does not change system routes or DNS. It therefore does not
validate an installed TUN session, browser proxy adoption, Windows Update download,
LAN name discovery, custom split DNS, Tailscale subnet routes or VPN recovery on
this computer. These scenarios are recorded in `QA_CHECKLIST.md` for device QA.
The source changes do not themselves update or restart the installed client.

## Routing references

Microsoft documents the update-specific domains separately from general Microsoft
services: [Windows connection endpoints](https://learn.microsoft.com/en-us/windows/privacy/manage-windows-11-endpoints).
The direct rules target update domains, rather than all `microsoft.com` traffic.

Tailscale documents its local MagicDNS resolver as `100.100.100.100`:
[Quad100](https://tailscale.com/docs/reference/quad100).
The generator enables that DNS server only when it finds an active external
Tailscale interface; the embedded Tailscale profile keeps its existing handling.

## Completed validation

The final Windows Qt build and the diagnostic Go binary both built successfully.
The generated update archive passed ZIP CRC validation and contains exactly the
four expected runtime files; its GUI executable matches the final build by SHA-256.
The updater was packaged but not applied to the running installation.

A second endpoint (`https://www.cloudflare.com/cdn-cgi/trace`) was tested through
13 selected servers. Four responded: Germany #2 (VLESS and Hysteria2), Switzerland,
and Netherlands. The nine repeated failures included all four provider Auto
endpoints. Local details are in `runtime-test/tunnel-recheck.json`.

## DNS regression follow-up

Confirmed with the real core: adding `detour: direct` to the external Tailscale
UDP DNS server fails initialization with `detour to an empty direct outbound
makes no sense`. Removing the detour starts successfully. A loopback DNS fixture
also verifies a `*.ts.net` query receives its real private address before FakeIP.
Both tests pass in `internal/boxmain/dns_regression_test.go`.

GitHub still lists v0.1.2 as the latest published release. The corrected candidate
is 0.1.3 on `codex/0.1.3-dns-regression`; it has not been published or installed.
The packaging guard rejects a 0.1.2 GUI labeled as 0.1.3 before staging mutation.
