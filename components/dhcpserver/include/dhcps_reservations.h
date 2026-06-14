/* Static DHCP reservation hooks for the vendored ESP-IDF DHCP server.
 *
 * This header is part of the locally vendored copy of the ESP-IDF
 * `dhcpserver` (see components/dhcpserver/dhcpserver.c). It exposes a small
 * resolver interface that lets the application map client MAC addresses to
 * fixed IPv4 addresses (static reservations) without forking esp_netif.
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward lookup: if @mac has a reservation, store its IPv4 address (network
 * byte order, as returned by esp_ip4addr_aton) in @out_ip and return true.
 * Return false when @mac has no reservation. */
typedef bool (*dhcps_resolve_mac_fn)(void *arg, const uint8_t mac[6], uint32_t *out_ip);

/* Reverse lookup: return true when @ip (network byte order) is reserved for a
 * MAC different from @mac. Used to keep reserved addresses out of the dynamic
 * pool so they are never handed to a non-reserved client. */
typedef bool (*dhcps_reserved_ip_fn)(void *arg, uint32_t ip, const uint8_t mac[6]);

/* Register (or clear, by passing NULL) the reservation resolver callbacks.
 * The firmware runs a single DHCP server, so a global resolver is sufficient.
 * @arg is passed back to both callbacks unchanged. */
void dhcps_set_reservation_resolver(dhcps_resolve_mac_fn lookup,
                                    dhcps_reserved_ip_fn ip_taken,
                                    void *arg);

#ifdef __cplusplus
}
#endif
