all:
	make -C ds4_module
	make -C ds4_read

clean:
	make clean -C ds4_module
	make clean -C ds4_read

init:
	-rmmod hid_ds4
	-rmmod hid_sony
	-insmod ds4_module/hid-ds4.ko

start: 
	make start -C ds4_read

exit:
	make exit -C ds4_read
	rmmod hid_ds4


