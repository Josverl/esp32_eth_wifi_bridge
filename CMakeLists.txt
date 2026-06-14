# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

set(PROJECT_VER "3.0.5")
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(esp32_eth_wifi_bridge)

# --- Vendored DHCP server with static MAC->IP reservations ---
# Replace the dhcpserver.c bundled inside the lwip component with our patched
# copy in components/dhcpserver so DHCP reservations are honoured. The patched
# source is compiled into the lwip target itself (inheriting lwip's private
# include dirs and compile definitions), so exactly one set of dhcps_* symbols
# ends up in the image.
idf_component_get_property(lwip_lib lwip COMPONENT_LIB)
if(lwip_lib)
    set(_bundled_dhcpserver "$ENV{IDF_PATH}/components/lwip/apps/dhcpserver/dhcpserver.c")
    set(_vendored_dhcpserver "${CMAKE_CURRENT_SOURCE_DIR}/components/dhcpserver/dhcpserver.c")
    get_target_property(_lwip_srcs ${lwip_lib} SOURCES)
    if(_bundled_dhcpserver IN_LIST _lwip_srcs)
        list(REMOVE_ITEM _lwip_srcs "${_bundled_dhcpserver}")
        set_property(TARGET ${lwip_lib} PROPERTY SOURCES ${_lwip_srcs})
        target_sources(${lwip_lib} PRIVATE "${_vendored_dhcpserver}")
        target_include_directories(${lwip_lib} PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/components/dhcpserver/include")
        message(STATUS "DHCP reservations: using vendored dhcpserver.c")
    else()
        message(WARNING "DHCP reservations: bundled dhcpserver.c not found in lwip "
                        "sources; static reservations will be inactive")
    endif()
endif()

# Force esp_app_desc.c to recompile on every build so that
# __DATE__/__TIME__ in the app descriptor stay current.
set(APP_DESC_OBJ "${CMAKE_BINARY_DIR}/esp-idf/esp_app_format/CMakeFiles/__idf_esp_app_format.dir/esp_app_desc.c.obj")
add_custom_target(force_app_desc_rebuild ALL
    COMMAND ${CMAKE_COMMAND} -E remove -f "${APP_DESC_OBJ}"
    COMMENT "Ensuring app descriptor timestamp is current"
)
add_dependencies(${PROJECT_NAME}.elf force_app_desc_rebuild)
