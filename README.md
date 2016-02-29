razos
=====

Razbit's (and Itara20's) operating system, RazOS.

This is our diploma work, an operating system trying to be as unix-like as is possible in our timeframe. Maybe one day this project (razos kernel and rlibc C library) is POSIX/C99 compliant, maybe.

#Setting up the developing environment
##Making cross-compiler
Start by making sure you have GCC-5.2.0 installed.
Then see the file cross-compiler for a quick how-to build a cross-GCC.

http://wiki.osdev.org/Building_GCC
http://wiki.osdev.org/GCC_Cross-Compiler

##Setting up NASM
```shell
$ sudo apt-get install nasm
```

##Setting up Bochs
First, download Bochs sources (we use version 2.6.8).
Make sure you have gtk2.0-dev, libncurses5-dev and libsdl-dev *SDL1* libraries.

Extract the tarball and cd to the bochs-2.6.8 directory. Then build Bochs:
```shell
$ ./configure --enable-smp \
              --enable-cpu-level=6 \
              --enable-all-optimizations \
              --enable-x86-64 \
              --enable-pci \
              --enable-vmx \
              --enable-debugger \
              --enable-disasm \
              --enable-debugger-gui \
              --enable-logging \
              --enable-fpu \
              --enable-3dnow \
              --enable-sb16=dummy \
              --enable-cdrom \
              --enable-x86-debugger \
              --enable-iodebug \
              --disable-plugins \
              --disable-docbook \
              --with-x --with-x11 --with-term --with-sdl
$ make
$ sudo make install
```

#Building and running
###Build the kernel
```shell
$ make kernel
```

###Build the rlibc and user-space
```shell
$ make tools
$ make init
```

##Run
Running the system requires root priviledges (see scripts update\_image.sh and run\_bochs.sh for more info)

To run Razos using Bochs
```shell
$ sudo make bochs
```
Or using QEMU
```shell
$ sudo make qemu
```

###Debugging in Bochs
See http://wiki.osdev.org/Bochs#Bochs_debugging_facilities

#Acknowledgements
Bran's kernel development tutorials. Brandon Friesen,
http://www.osdever.net/bkerndev/Docs/title.htm

JamesM's kernel development tutorials. James Molloy,
http://jamesmolloy.co.uk/tutorial_html/

OsDev forums and Wiki,
http://wiki.osdev.org

Charlie Somerville (his project, Radium),
https://github.com/charliesome/radium


Cheers,
Razbit + Itara
