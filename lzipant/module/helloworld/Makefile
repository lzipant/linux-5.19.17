CC = gcc                # Compiler
CFLAGS = -Wall -g       # Compiler flags

ifneq ($(KERNELRELEASE),)
obj-m := helloworld.o
else
PWD := $(shell pwd)
# KDIR := /lib/modules/6.8.0-39-generic/build
KDIR := /root/projects/linux/linux-5.19.17
#KDIR := /lib/modules/`uname -r`/build
all:
	make -C $(KDIR) M=$(PWD)
clean:
	rm -rf *.o *.ko *.mod.c *.symvers *.c~ *~
endif


test:
	$(CC) $(CFLAGS) -o test.a test.c