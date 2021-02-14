# Dictionary Compiler

In order to get fast dictionary read speed from a not very powerful MCU, the dictionary need to be compiled to a more MCU friendly format. This generally mean a well structured binary format with as little as variable sized data as possible. We don't want to have dynamic sized data because that either means we need to have terminating markers (think null terminated strings in C) and pointers (which can be good a lot of times, but also wastes space in unneeded times). In this case, the size of a single stroke is fixed, as it can be represented as a single 23 bit integer. But a whole sequence of strokes would be of dynamic size. The entry would have to be dynamic sized.

## Version 2

In the version 2 design, a lot of the work is to be handled by the MCU (including orthographic stuff), so the compiler was only responsible for bootstrapping the dictionary. The general algorithm of compiling is the same as version 1, and the rest of the dictionary structure can be found in the firmware documentation. The only notable difference would be that the binary is directly converted to UF2 for flashing.

## Version 1

The compiler is implemented in Rust, and is made up of 3 main parts: orthographic transform, dictionary compilation, and dictionary downloading.

### Orthographic Transform

Plover does this in the runtime, and this is needed to correctly bridge the gap between syllables and the actual spelling. For example, if you stroke `RUPB/-G` the output should be "running" and not "runing". My implementation follows [Dotterel](https://github.com/nimble0/dotterel) (which is a steno engine on android devices), and the orthographic rules data is converted from the one used in Dotterel. It uses some regex rules that match and replace the word when it's followed by a suffix (i.e. an entry that has no space before it) and a big dictionary to check if the result is a real word or not. Although it's possible to put a regex engine in a AVR chip, it's not that easy to implement it. And it's only the rules themselves, the word list would require a lot more memory to handle. In addition, we also need to check if several regexs match or not, which is very expensive.

Hence, I chose to do it at the dictionary level. This can save the amount of resources required to fix orthographic problems, but also only the first level rule application is done, so some repeated application of suffixes in this implementation will be incorrect. The compiler first load the dictionary, filter out all the suffixes, apply them to each word in the dictionary, and at the end check if the result is a word or not. If it is, then we add the new stroke and entry into the dictionary.

### Dictionary Compilation

Once we have a dictionary that has undergone correct orthographic transformation, we can compile it into binary form. The dictionary strucutre will be discussed in detail in the firmware documentation, but basically the JSON dictionary is converted to a prefix tree in binary.

When the entries are parsed, the commands, key codes, and raw text are concatenated together into a single chunk according to the commands. For every entry, a new state is created, and the commands will mutate the state, which goes to influence how the next texts are concatenated. At the end, this will output one piece of text and some attributes. This is also done to reduce the amount of resources needed at runtime to parse all the commands.

Finally, all the nodes are fed into a big binary buffer for output. When each node is added, its entry text and attributes are converted to binary and added to the buffer. Its children are added recursively, and the starting address of the node is recorded. The strokes and the addresses of the nodes that they lead to are then put into a simple linear probing hash map, which can be converted to binary easily.

### Dictionary Downloading

Currently, the dictionary downloading mechanism is implemented in the raw HID interface that QMK provides, so is is is this part of the compiler implements a communication protocol that talks to the MCU and transfers the compiled dictionary over. The current protocol uses a packet length of 64 bytes. Every packet contains a header, operation ID, some parameters, and the CRC of the packet. Every write contains 28 groups of data length and a CRC. For each of these groups, a packet with only raw data (no header, operation ID etc.) would be sent, and the ACK from the MCU would be waited for.
