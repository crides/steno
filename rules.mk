MCU = atmega32u4
F_CPU = 8000000
ARCH = AVR8
F_USB = $(F_CPU)
BOOTLOADER = caterina

# SRC += File.cpp SD.cpp utility/Sd2Card.cpp utility/SdFile.cpp utility/SdVolume.cpp

MOUSEKEY_ENABLE = yes
EXTRAKEY_ENABLE = yes
CONSOLE_ENABLE = yes
STENO_ENABLE = yes
FORCE_NKRO = yes
BLUETOOTH = AdafruitBLE
COMBO_ENABLE = yes
