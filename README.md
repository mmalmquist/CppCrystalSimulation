# CppCrystalSimulation
My C++ (with GTK+) implementation of a crystal simulation<br>
Requires GTK+ 3.

## Build

### GNU/Linux
Any of:
+ <code>$ ./make_build debug</code>
+ <code>$ ./make_build release</code>

### Windows
MSYS2 should be used to get a working environment.<br>
In the MinGW command prompt you can use any of
+ <code>$ ./make_build debug -G"MSYS Makefiles"</code>
+ <code>$ ./make_build release -G"MSYS Makefiles"</code>

## Run
- Supports GUI and CLI mode.
- Supports different sizes
<br>
<code>$ ./build/CppCrystalSimulation mode=[mode] size=[size]</code>
<br>
Note that on Window you should use the MinGW command prompt to run.

