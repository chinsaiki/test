obj-m += procsview.o

#KERNEL_VERSION="3.10.0-693.el7.x86_64"

KDIR := /lib/modules/$(shell uname -r)/build
#KDIR := /lib/modules/$(KERNEL_VERSION)/build
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

run:
	clear
	insmod procsview.ko
	grep RTOAX /var/log/messages
	rmmod procsview
