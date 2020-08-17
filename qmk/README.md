# Firmware

## Overview

Shortly after I built the [first version](../pcb/README.md#Version-1) of the steno board, I got the idea of building a plug-and-playable steno keyboard. The idea was that I can compile the dictionary into a special format that's easier for the MCU to process, and load it on to a SD card. When I type on the keyboard, the MCU then doesn't send the strokes to the PC for Plover to process, but rather process the translation by itself, and send the actual key codes to the PC. Note that I originally planned to do this on a ATMega32u4, which runs on only 8MHz, 2.5kB of RAM, and 28kB of available application program space (the other 4k devoted to bootloader). The currently implementation runs on the [32u4 Bluefruit feather](https://www.adafruit.com/product/2829) fine, with most of the Plover dictionary formats compatible (including all the commands that don't include retro- functions). Currently, only text input is possible, and one would need to load a new dictionary from a PC in order to change the dictionary.

## How do I use it?

### Requirements

- A compatible chip or controller board that uses these chips: ATMega32u4 or nRF52840
- Some storage: A SDCard connector or a QSPI external flash with at least 8MB (I'm using MT25QL128ABA1ESE)
- A keyboard
- QMK toolchain

Configure the firmware. There are 2 blocks at the top of `rules.mk`, comment one of them and uncomment the other. One of them is for AVR, and the other is for the nRF. Configure the matrix in `config.h`, and modify keymaps in `keymaps/default/keymap.c` if needed. Also configure in `config.h` what storage you are using (`USE_SPI_FLASH`). And because of design differences, if you're using a QSPI flash, uncomment `#define STENO_PHONE`.

Put/Link this directory in/from `qmk_firmware/keyboards`. If you're using nRF52840, you need the [`nrf52` branch of sekigon's fork](https://github.com/sekigon-gonnoc/qmk_firmware/tree/nrf52). Compile and load the firmware, and you will need a compiled dictionary. You can either get one from the repo as `steno.bin` (which is my current compiled dictionary), or grab the [compiler](../compiler) and compile your own. Load the dictionary by either copying to the SDCard, or use the `download` command in the compiler if using a QSPI flash.

## Implementation

This firmware uses the [QMK firmware](https://qmk.fm), but strips off any useless features for a fully steno board, like layers. It uses the steno driver, but intercepts all the strokes by using `send_steno_chord_user`. Instead of sending them off to the host, the firmware performs several look ups in the dictionary, parses the entry, and outputs the needed key strokes. Loading dictionaries is done by implementing a raw HID device and receiving packets from a PC using the compiler, and this is currently only available for AVR, and I'm still working on a way to load dictionaries on ARM using Mass Storage Class.

The dictionary format will be documented in more detail in the [compiler documentation](../compiler/README.md), but basically the format is designed so that the MCU don't have to do a lot of work in order to look up some entry. The dictionary is organized as a tree, with each edge being a single stroke, and a whole path being a series of strokes, possibly leading to a entry, which would be stored in the node if there's one. The reason why it's a tree is because it's much easier to store and handle fixed sized data, as one can store them in place, without any kinds of pointers and size markers. The size of a stroke in this case is fixed, as it's a 23 bit integer corresponding to the 23 different keys on the steno board. In the early prototyping stage, the children of a node is organized as an array of pairs of strokes and the node addresses, sorted by the input stroke, so that the array can be binary searched, and the compiled dictionary would be very compact. Currently, in order to have better overall performance and make room for supporting in place dictionary editing, the children are organized as a hash map. Hash maps require empty space to perform well, and although that may seem like wasted space, it can also be used to cheaply add or remove entries in the future.

In order to support Plover commands but to reduce the complexity, those are implemented as attributes of a entry. They are simply flags in the compiled dictionary, and they are evaluated when the outputs are you evaluated, changing the output or changing the state of the system which will go on affecting the next output along with the next entry attributes.

When searching for a entry, the firmware has an internal array of nodes. Each time a user inputs a stroke, the firmware will look up the stroke in the children of those nodes and the root node. If any new nodes are found, those become the new search array. Out of these nodes, the one with the longest match with an entry will be used, and the output will be evaluated and sent to the host.

## Current Issues

- Sometimes the firmware goes crazy and seems to get stuck in an infinite loop, although there doesn't seem to be anything in my firmware that creates one directly. It could be one related to unhandled state transitions or something like that.
- The nRF firmware sometimes will read garbage data. I suspected the issue was that the timing requirements of the QSPI protocol wansn't matched, but fixing that doesn't seem to fix the core issue. I also suspected that it might be a IRQ priority (which doesn't seem like it) or that my traces don't handle high frequencies like this (32MHz, not much, but I haven't dealt with traces that require this much either) very well.

Also see [current issues in the compiler](../compiler/README.md#Current Issues)

## Future Work

- Add optimization that reuse the common parts of words. Currently, whenever a new entry is inputted, the old entry would always be deleted, regardless whether there's a common part or not. This is because, in order to save memory, only the length of the previous stroke is recorded, and the easiest way would just to delete what we had entered before and reenter the whole thing. The easiest way to add the optimization is to make a buffer for the previous output, and compare how much has changed. This however would not work when undos have occurred. The next easiest solution would be to read in the previous entry every time we want to output something, and evaluate the entry and do the comparison. This however, might cause performance issues. The most complete way would be to store the entire output in the previous history entry.
- Make it easier to load the dictionary from a PC. Currently dictionary loading only works with the AVR firmware, as it uses raw HID for the process, and that's not available at the moment in the nRF QMK fork. A solution that can work on both would be to implement a USB Mass Storage device, make it into a fake FAT device (like how Adafruit would load the firmware through a MSC), and load the dictionary that way. It would also make it potentially less environment dependent.
- Implement in-place dictionary editing. The required architecture should be in place. Hash maps should be fairly easy to modify, as there are already space leftover for performance reasons, and the empty space are erased.
