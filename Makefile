##
## Makefile for controller_driver
##

TARGET	= controller_driver

obj-m	+= $(TARGET).o

CURRENT = $(shell uname -r)
KDIR	= /lib/modules/$(CURRENT)/build

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
