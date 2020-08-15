# Dictionary Compiler

## Overview

In order to get fast dictionary read speed from a not very powerful MCU, the dictionary need to be compiled to a more MCU friendly format. This generally mean a well structured binary format with as little as variable sized data as possible. We don't want to have dynamic sized data because that either means we need to have terminating markers (think null terminated strings in C) and pointers (which can be good a lot of times, but also wastes space in unneeded times). In this case, the size of a single stroke is fixed, as it can be represented as a single 23 bit integer. But a whole sequence of strokes would be of dynamic size. The entry would have to be dynamic sized.
