razos
=====

Razbit's (Very basic) operating system, RazOS.

TBH, I really have no specific goals for this project..
I hope that when I'll build the RPU, a computer made from ttl chips,
I could run this system on it. So, the code will have to be specific for
the RPU platform (8086-ish), and the platform will have to be able to
run code this complex. But, we'll see.. This is a hobby for which I don't
have much time..

#Setting up the developing environment
My host system is Ubuntu/Debian based and I use the Netwide Assembler as the
assembler for this project (until transitioning to rasm one day, when the RPU or
a simulator of it is done). The simulator for now is Bochs (wx).

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

#Building and running in Bochs
##Build
```shell
$ make
```

##Run in Bochs
Running the system requires root priviledges (see scripts for more info)
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
kernel, since in a way Bochs has already opened/started executing the kernel.

See these two links for more info on debugging an OS in bochs with GDB:
http://www.cs.princeton.edu/courses/archive/fall09/cos318/precepts/bochs_gdb.html
http://www.csee.umbc.edu/~cpatel2/links/310/nasm/gdb_help.shtml

#Acknowledgements
Bran's kernel development tutorials. Brandon Friesen,
http://www.osdever.net/bkerndev/Docs/title.htm

JamesM's kernel development tutorials. James Molloy,
http://jamesmolloy.co.uk/tutorial_html/

Cheers,
Razbit
