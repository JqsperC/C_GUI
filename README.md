## Selfmade IMGUI in C99

Repository for learning and making my own immediate mode GUI system in C.

For now it uses SDL3/SDL3_ttf as the only dependencies.

## Building

Building the project using `make` creates a `build` folder containing all the object files.
The Makefile compiles `.c` files from all subfolders into object files and links them all against `main.o`. Additional libraries can be linked by appending to the Makefiles `LLIBRARIES` entry.
