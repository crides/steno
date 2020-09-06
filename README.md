# Steno

This repo contains everything I designed and implemented that's related to stenography. You can find the PCB files for my steno keyboard in the `pcb/` directory, and the custom steno firmware and the dictionary compiler in the `qmk/` and `compiler/` directories.

Check out the [boards](pcb/README.md) and the [firmware](qmk/README.md) ([dictionary compiler](compiler/README.md) needed).

## What's Steno(graphy)?

If you don't know what stenography is, it's a short hand system primarily used by court reporters to input at a very high speeds. The English stenography system uses a special keyboard with only 23 keys on it, and the keys are meant to be pressed at the same time, producing combinations called chords. When combined together and translated through an engine, it can be used to input at very high speeds (professional stenographers need to reach above 240 wpm). Due to the price of commercial steno machines, free and open source alternatives started appearing, the most prominent among which is [Plover](https://github.com/openstenoproject/plover), which acts as a keyboard emulator that can translate chords from a \$20 gaming keyboard into words efficiently. A lot of ideas here are inspired by Plover.
