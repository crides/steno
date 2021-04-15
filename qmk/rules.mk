# Readonly dictionary. Dictionary editing is disabled
STENO_READONLY = no
# No display available for UI. Implies STENO_READONLY
STENO_NOUI = no
# No mass storage device enabled
STENO_NOMSD = no

MCU = atmega32u4
F_CPU = 16000000
ARCH = AVR8
F_USB = $(F_CPU)
BOOTLOADER = atmel-dfu
EXTRAKEY_ENABLE = no

SRC += hist.c stroke.c orthography.c
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

MOUSEKEY_ENABLE = no
VIRTSER_ENABLE = no
RAW_ENABLE = no
CONSOLE_ENABLE = yes
STENO_ENABLE = yes
COMBO_ENABLE = no
OLED_DRIVER_ENABLE = no
UNICODE_ENABLE = yes

EXTRAFLAGS += -flto
LTO_ENABLE = yes
GRAVE_ESC_ENABLE = no
MAGIC_ENABLE = no
SPACE_CADET_ENABLE = no
