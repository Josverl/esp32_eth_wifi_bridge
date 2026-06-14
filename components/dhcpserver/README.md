# Vendored DHCP server (static reservations)

`dhcpserver.c` in this directory is a **vendored copy of the ESP-IDF DHCP
server** taken from ESP-IDF **v5.3.2**
(`components/lwip/apps/dhcpserver/dhcpserver.c`, Apache-2.0). It carries a
small local patch that adds support for **static MAC→IP reservations**.

## Why vendor it?

ESP-IDF's bundled DHCP server exposes no public API for per-MAC static
reservations (`dhcps_set_option_info()` only sets global options such as the
gateway, DNS and lease time). Enforcing reservations therefore requires
intercepting the address-selection logic inside the server itself.

## What was changed

All local changes are marked with comments containing `Vendored extension`:

1. `#include "dhcps_reservations.h"` and a global resolver registered via
   `dhcps_set_reservation_resolver()`.
2. In `parse_msg()`: before the dynamic pool search, if the client MAC has a
   reservation the fixed address is assigned (and may sit outside the dynamic
   pool).
3. In `parse_msg()`: the dynamic allocator skips any address that is reserved
   for a different MAC, so reserved addresses are never handed out to other
   clients.

## How the override is wired

This component registers only the public header (`include/dhcps_reservations.h`).
The patched `dhcpserver.c` is compiled **into the `lwip` component**, replacing
lwip's bundled copy. That swap happens in the project's top-level
`CMakeLists.txt` (search for "Vendored DHCP server"). This keeps exactly one
definition of the `dhcps_*` symbols in the final binary.

## Updating ESP-IDF

When upgrading ESP-IDF, re-sync this file from the matching IDF tag and
re-apply the `Vendored extension` blocks, then rebuild and re-test
reservations. Update the version noted above.
