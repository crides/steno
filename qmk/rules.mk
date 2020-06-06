MCU = atmega32u4
F_CPU = 8000000
ARCH = AVR8
F_USB = $(F_CPU)
BOOTLOADER = caterina

SRC += flash.c hist.c stroke.c sdcard/fat.c sdcard/partition.c sdcard/sd_raw.c sdcard/byteordering.c

MOUSEKEY_ENABLE = no
EXTRAKEY_ENABLE = no
CONSOLE_ENABLE = yes
STENO_ENABLE = yes
# BLUETOOTH = AdafruitBLE
COMBO_ENABLE = no
# OLED_DRIVER_ENABLE = yes

EXTRAFLAGS += -flto
