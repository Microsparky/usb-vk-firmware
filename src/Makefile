PROJECT = custom-blink
BUILD_DIR = bin

SHARED_DIR = ../inc
CFILES = main.c
CFILES += system.c button.c dotstar.c encoder.c console.c usb_hid_consumer.c
AFILES += 

# TODO - you will need to edit these two lines!
DEVICE = stm32f401rc

# OPT - defaults to -Os. Use -O0 (no optimization)
OPT = -O0
LDLIBS = -lm

# You shouldn't have to edit anything below here.
VPATH += $(SHARED_DIR)
INCLUDES += $(patsubst %,-I%, . $(SHARED_DIR))
OPENCM3_DIR=../libopencm3

include $(OPENCM3_DIR)/mk/genlink-config.mk
include ../rules.mk
include $(OPENCM3_DIR)/mk/genlink-rules.mk
