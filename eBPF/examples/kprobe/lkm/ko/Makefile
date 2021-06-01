obj-m   += lkm_driver.o

KDIR    := /lib/modules/$(shell uname -r)/build

all:
	make -C $(KDIR) M=$(PWD) modules

install:
	insmod lkm_driver.ko

uninstall:
	rmmod lkm_driver

test:
	make install
	make uninstall
	dmesg | tail -2

clean:
	make -C $(KDIR) M=$(PWD) clean
