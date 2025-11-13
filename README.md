## Selfmade IMGUI in C99

Learning to make GUI from scratch in C99

Rendering: software renderer into an RGBX bitmap

Dependencies: 
- SDL: opening a window, getting input events, drawing bitmap to window
- Soon(TM) FreeType: generating bitmap font for making custom ASCII text renderer

## Building

Building the project using `make` creates a `build` folder containing all the object files.
The Makefile compiles `.c` files from all subfolders into object files and links them all against `main.o`. Additional libraries can be linked by appending to the Makefiles `LLIBRARIES` entry.
