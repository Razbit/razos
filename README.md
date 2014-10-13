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
Cheers,
Razbit
