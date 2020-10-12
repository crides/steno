MCU = atmega32u4
F_CPU = 16000000
ARCH = AVR8
F_USB = $(F_CPU)
BOOTLOADER = atmel-dfu
EXTRAKEY_ENABLE = no
SRC += analog.c

# NRF_DEBUG = no
# MCU_FAMILY = NRF52
# MCU  = cortex-m4
# ARMV = 7
# MCU_LDSCRIPT = nrf52840
# MCU_SERIES = NRF52840
# NRFSDK_ROOT := $(NRFSDK15_ROOT) #Path to nRF SDK v15.0.0
# CUSTOM_MATRIX = yes
# EXTRAKEY_ENABLE = yes

SRC += spi.c lcd.c lcd_font.c flash.c hist.c stroke.c sd/pff.c sd/diskio.c

MOUSEKEY_ENABLE = no
VIRTSER_ENABLE = no
RAW_ENABLE = yes
CONSOLE_ENABLE = yes
STENO_ENABLE = yes
COMBO_ENABLE = no
OLED_DRIVER_ENABLE = no

EXTRAFLAGS += -flto
