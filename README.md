## Pong in WASM ([Web-Assembly](https://webassembly.org/)), Native OpenGL and NCurses (TUI).

This contains the worst AI you have ever seen. You have been warned.

The WASM version is in C++ because the keyboard function refuses to work after being compiled by Emscripten for some reason I'm not aware of.

To control your paddle in the WASM and native OpenGL versions, use WASD. To control it in the ncurses version, use the arrow keys. (Sorry for this.)

To compile the WASM version, you'll need emscripten.
