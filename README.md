# Paint in C

Trying to code an application like MS Paint in C, without using AI to entertain myself and learn the SDL library.

## Features

* **Freehand Drawing:** Draw continuously with interpolation (no gaps when moving fast).
* **Color Picker:** Select from 16 predefined colors.
* **Brush Size Control (Scroll the mouse wheel):** Adjust brush size dynamically using the mouse wheel.
* **Undo/Redo System (CTRL + Z / CTRL + Y):** Robust circular buffer implementation.
* **Save/Load (S / L):** Save your artwork to BMP and load it back.
* **Highly Customizable:** Easily change constants like history depth (`MAX_UNDO_STEPS`), frame rate, or available colors directly in `paint.h`.

## Dependencies

To build and run this project, you need **GCC** and the **SDL2** development library installed.

### Linux (Debian/Ubuntu/Mint)
```bash
sudo apt-get update
sudo apt-get install build-essential libsdl2-dev
```

### How to run
I have included a `Makefile` to automate the compilation process.

### Option 1: Using Make (Recommended)

* **Compile and Run (Easiest):**
    This command compiles the code and immediately starts the application.
    ```bash
    make run
    ```

* **Compile only:**
    This creates the `paint` executable but does not run it.
    ```bash
    make
    ```

* **Clean up:**
    Removes the compiled executable to start fresh.
    ```bash
    make clean
    ```

### Option 2: Manual Compilation

If you cannot use `make`, you can compile it manually using GCC:

```bash
gcc main.c -o paint -Wall -Wextra $(sdl2-config --cflags --libs)
./paint
```