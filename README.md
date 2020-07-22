# Steno

This repo contains everything I designed and implemented that's related to stenography. You can find the PCB files for my steno keyboard in the `pcb/` directory, and the custom steno firmware and the dictionary compiler in the `qmk/` and `compiler/` directories.

## PCB

The version 1 board is just a experimental board, with a small form factor while keeping the 11 column of keys needed. The number bar is missing, just like the Georgi, and the number keys are placed at either side of the vowels. As I was just starting to practice steno, I didn't know much about the dimension of a steno keyboard that I would feel comfortable using, so I simply used a ortholinear layout. In order to keep the form factor small, I tried to put the MCU under the PCB, so that it doesn't take any extra space on the PCB itself. But since I'm not used to doing that, it caused some issues when I later tried to modify the board and in normal usage.

Version 2 is a much better improvement over the first. This time I chose a split layout, but on the same board so I may use it on the go. The 2 halves are rotated 20 degrees each, so my hands won't have to bend much since it's not a actual split board. Like my split keyboard, this version also includes high levels of columnar stagger. I also put the MCU back to a normal position, so I don't have to worry about the underside of the keyboard anymore.

## Firmware

Shortly after I built the first version of the steno board, I got the idea of building a plug-and-playable steno keyboard. The idea was that I can compile the dictionary into a special format that's easier for the MCU to process, and load it on to a SD card. When I type on the keyboard, the MCU then doesn't send the key presses to the PC for Plover to process, but rather process the key presses by itself, and send the actual key codes to the PC. Note that I originally planned to do this on a
