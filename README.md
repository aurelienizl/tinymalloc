# MMAP Memory Allocator

## Overview

The following project is a custom memory allocation library implemented in C. It leverages the `mmap` system call to allocate memory dynamically, providing a custom alternative to standard memory allocation methods. This allocator is NOT EXPECTED to be used in production systems. 

## Features

- **Bit bucket implementation**: This allocator is based on the bit bucket implementation.
- **Balanced performance and memory usage**: This memory allocator use an alignment to a multiple of 16. The performance limitation is due to the usage of a linked list. 
- **Recycler Mechanism**: Every memory free is added to a recycler linked list. When a page does not contain any allocation, the page is freed. 
- **Thread-Safe**: This memory allocator is Thread Safe. 

## Getting Started

### Prerequisites

- GCC compiler
- Linux-based operating system (due to `mmap` usage)
- `make` for building the project

### Building the Library

The criterion testsuite is required if you want to launch the associated tests. 


## Run Locally

Clone the project

```bash
  git clone [URL]
```

Go to the project directory

```bash
  cd [FOLDER]
```

Build the library

```bash
  make
```

Launch a binary using the library

```bash
  LD_PRELOAD=./libmalloc.so [COMMAND]
```

Debug the library using gdb 

```bash
  gdb --args env LD_PRELOAD=[LIBRARY PATH] [COMMAND]
```

Replace `[LIBRARY PATH]` with the path to your library and `[COMMAND]` with the command to run your program. This command sets `LD_PRELOAD` within the GDB environment, allowing you to debug your program with the custom allocator loaded.


## Authors

- [@aurelien_izl](https://github.com/aurelienizl)

