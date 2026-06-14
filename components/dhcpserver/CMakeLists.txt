# Vendored ESP-IDF DHCP server with static MAC->IP reservation support.
#
# This component intentionally registers ONLY the public reservation header
# (dhcps_reservations.h). The patched dhcpserver.c is compiled into the lwip
# component instead of lwip's bundled copy; that swap is performed in the
# top-level CMakeLists.txt so that exactly one definition of the dhcps_*
# symbols exists in the final image.
idf_component_register(INCLUDE_DIRS "include"
                       REQUIRES lwip)
