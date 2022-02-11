# Steno

If you are here, you are probably looking for my embedded steno work. You can find the PCB design and production files for my steno keyboards (mostly known as the batwing or just bat) in the `pcb/` directory, and the embedded steno firmware (Kolibri) and the dictionary compiler in the `qmk/` and `compiler/` directories.

The detailed info for each can be found in their own READMEs, [boards](pcb/README.md) [firmware](qmk/README.md) and [dictionary compiler](compiler/README.md).

## What's Steno(graphy)?

If you don't know what stenography is, it's a short hand system primarily used by court reporters and captioners for note taking at a very high speeds. The English stenography system uses a special keyboard with only 23 keys on it, and the keys are meant to be pressed at the same time, producing combinations called chords. When combined together and translated through an engine, it can be used to input at very high speeds (professional stenographers need to reach above 225 wpm). Due to the price of commercial steno machines, free and open source alternatives started appearing, the most prominent among which is [Plover](https://github.com/openstenoproject/plover), which acts as a keyboard emulator that can translate chords from a \$20 gaming keyboard into words efficiently. A lot of ideas here are inspired by Plover.

If you want to go a bit more in depth, you should check out [this video](https://www.youtube.com/watch?v=JsRTTD9k2ME) by Aerick, or this [PyCon talk](https://www.youtube.com/watch?v=Wpv-Qb-dB6g) by Mirabai Knight, the creator of Plover.
