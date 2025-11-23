## Selfmade IMGUI in C99

Learning to make GUI from scratch in C99

- CPU renderer
- IMGUI (Ryan Fleury IMGUI series https://www.rfleury.com/p/posts-table-of-contents)

Dependencies: 
- SDL: opening a window, getting input events, drawing bitmap to window
- FreeType: generating character bitmaps for ASCII text renderer

## Building

Building the project using `make` creates a `build` folder containing all the object files.

Debug build: `make debug`

Optimized build: `make optimization`

The Makefile compiles `.c` files from all subfolders into object files and links them all against `main.o`. Additional libraries can be linked by appending to the Makefiles `LLIBRARIES` entry.
