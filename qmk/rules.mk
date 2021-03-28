MCU = atmega32u4
F_CPU = 16000000
ARCH = AVR8
F_USB = $(F_CPU)
BOOTLOADER = atmel-dfu
EXTRAKEY_ENABLE = no

SRC += spi.c lcd.c lcd_font.c flash.c hist.c stroke.c dict_editing.c scsi.c ghostfat.c freemap.c orthography.c

MOUSEKEY_ENABLE = no
VIRTSER_ENABLE = no
RAW_ENABLE = no
CONSOLE_ENABLE = yes
STENO_ENABLE = yes
COMBO_ENABLE = no
OLED_DRIVER_ENABLE = no
UNICODE_ENABLE = yes
MSC_ENABLE = yes

EXTRAFLAGS += -flto
LTO_ENABLE = yes
GRAVE_ESC_ENABLE = no
MAGIC_ENABLE = no
SPACE_CADET_ENABLE = no
