razos
=====

Razbit's (very basic) operating system, RazOS.

~~TBH, I really have no specific goals for this project..
I hope that when I'll build the RPU, a computer made from ttl chips,
I could run this system on it. So, the code will have to be specific for
the RPU platform (8086-ish), and the platform will have to be able to
run code this complex. But, we'll see.. This is a hobby for which I don't
have much time..~~

Now this project has changed; it is no longer a weekend project but rather the diploma work of Itara20 and me.. So, now there actually are goals (discussed later).

#Setting up the developing environment
##Making cross-compiler
Start by making sure you have GCC-5.2.0 installed. Then see the file cross-compiler for a quick how-to build a cross-GCC.

http://wiki.osdev.org/Building_GCC
http://wiki.osdev.org/GCC_Cross-Compiler

##Setting up NASM
```shell
$ sudo apt-get install nasm
```

##Setting up Bochs
```shell
$ sudo apt-get install libwxgtk2.8-dev bochs
$ cd /usr/share/bochs
$ sudo wget https://github.com/larsr/bochs/blob/master/bios/VGABIOS-elpin-2.40
```

#Building and running
##Build the kernel
```shell
$ make kernel
```

##Run´
Running the system requires root priviledges (see scripts for more info)
To run Razos using Bochs
```shell
$ sudo make bochs
```
Or using QEMU
```shell
$ sudo make qemu
```

You can manually use the scripts, too
```shell
$ sudo ./update_image.sh
$ sudo ./run_bochs.sh
```

#Debugging using Bochs and GDB
To debug the kernel using GDB, first remove the # from the bochsrc file (last
line). Then, start Bochs and in another terminal start gdb:
```shell
$ sudo ./run_bochs.sh
```
```shell
$ cd src
$ gdb kernel
```

In GDB, type
```shell
target remote localhost:1234
```
Now you can use GDB like you normally would, pretty much the only difference
being that you can't type 'run', but rather 'continue' to start executing the
kernel, since in a way Bochs has already started executing the kernel.

See these two links for more info on debugging an OS in Bochs using GDB:
http://www.cs.princeton.edu/courses/archive/fall09/cos318/precepts/bochs_gdb.html
http://www.csee.umbc.edu/~cpatel2/links/310/nasm/gdb_help.shtml

#Acknowledgements
Bran's kernel development tutorials. Brandon Friesen,
http://www.osdever.net/bkerndev/Docs/title.htm

JamesM's kernel development tutorials. James Molloy,
http://jamesmolloy.co.uk/tutorial_html/

OsDev forums and Wiki,
http://wiki.osdev.org

Charlie Somerville (His project, Radium),
https://github.com/charliesome/radium


Cheers,
Razbit + Itara
