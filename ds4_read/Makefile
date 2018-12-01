all:
	gcc -o device_read device_read.c -lxdo
	gcc -o event_gen event_gen.c
	gcc -o main main.c
clean:
	rm device_read
	rm event_gen
	rm main
start: main
	./main
	
