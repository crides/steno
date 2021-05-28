THIS_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
include $(THIS_DIR)/config.mk

SRC += hist.c stroke.c
SRC += impl/qmk/hooks.c impl/qmk/spi.c impl/qmk/flash.c
ifeq ($(STENO_NOUI),yes)
	STENO_READONLY = yes
	CFLAGS += -DSTENO_NOUI
else
	SRC += impl/qmk/disp.c impl/qmk/lcd.c impl/qmk/lcd_font.c
endif

ifeq ($(STENO_READONLY),yes)
	CFLAGS += -DSTENO_READONLY
else
	SRC += dict_editing.c freemap.c
endif

ifeq ($(STENO_NOMSD),yes)
	CFLAGS += -DSTENO_NOMSD
	MSC_ENABLE = no
else
	SRC += scsi.c ghostfat.c
	MSC_ENABLE = yes
endif

ifeq ($(STENO_STROKE_DISPLAY),yes)
	CFLAGS += -DSTENO_STROKE_DISPLAY
endif

ifeq ($(STENO_NOUNICODE), yes)
	CFLAGS += -DSTENO_NOUNICODE
endif

ifeq ($(STENO_NOORTHOGRAPHY),yes)
	CFLAGS += -DSTENO_NOORTHOGRAPHY
else
	SRC += orthography.c
endif

STENO_DEBUG := $(filter hist stroke flash dicted, $(STENO_DEBUG))
STENO_LOG_BACKEND := $(filter flash console, $(STENO_LOG_BACKEND))

ifneq (, $(STENO_LOG_BACKEND))
	CFLAGS += -DSTENO_DEBUG

ifneq (, $(findstring hist, $(STENO_DEBUG)))
	CFLAGS += -DSTENO_DEBUG_HIST
endif

ifneq (, $(findstring stroke, $(STENO_DEBUG)))
	CFLAGS += -DSTENO_DEBUG_STROKE
endif

ifneq (, $(findstring flash, $(STENO_DEBUG)))
	CFLAGS += -DSTENO_DEBUG_FLASH
endif

ifneq (, $(findstring dicted, $(STENO_DEBUG)))
	CFLAGS += -DSTENO_DEBUG_DICTED
endif
endif

ifneq (, $(findstring flash, $(STENO_LOG_BACKEND)))
	SRC += flog.c
	CFLAGS += -DSTENO_FLASH_LOGGING
endif

ifneq (, $(findstring console, $(STENO_LOG_BACKEND)))
	CONSOLE_ENABLE = yes
endif

MCU = atmega32u4
F_CPU = 16000000
ARCH = AVR8
F_USB = $(F_CPU)
BOOTLOADER = atmel-dfu
EXTRAKEY_ENABLE = no

MOUSEKEY_ENABLE = no
RAW_ENABLE = no
COMBO_ENABLE = no
OLED_DRIVER_ENABLE = no
UNICODE_ENABLE = yes

EXTRAFLAGS += -flto
LTO_ENABLE = yes
GRAVE_ESC_ENABLE = no
MAGIC_ENABLE = no
SPACE_CADET_ENABLE = no
