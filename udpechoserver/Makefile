# name of your application
APPLICATION =  udpechoserver

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_sock_udp
USEMODULE += gnrc_udp

USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif

USEMODULE += gnrc_rpl
USEMODULE += auto_init_gnrc_rpl

IPV6_STATIC_LLADDR ?= '"fe80::cafe:cafe:cafe:1"'
CFLAGS += -DGNRC_IPV6_STATIC_LLADDR=$(IPV6_STATIC_LLADDR)

include $(RIOTBASE)/Makefile.include
