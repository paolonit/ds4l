##
## Makefile for hid-ds4
##

ifneq ($(KERNELRELEASE),)
	obj-m	:= hid-ds4.o
else
	CURRENT = $(shell uname -r)
	KDIR?=/lib/modules/$(CURRENT)/build
	PWD:=$(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

endif
