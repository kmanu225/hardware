APPLICATION = tp1-led
  

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += xtimer

BOARD ?= iotlab-m3


# Add extra modules here



# Add the required modules here
USEMODULE += lps331ap
USEMODULE += lsm303dlhc
USEMODULE += isl29020
USEMODULE += l3g4200d
RIOTBASE ?= $(CURDIR)/../../riot/RIOT


# Add extended thread information
DEVELHELP ?= 1

include $(RIOTBASE)/Makefile.include
