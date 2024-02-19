main.elf: main.o retardo.o
	gcc -g main.o retardo.o -o main.elf -lwiringPi

main.o: main.c
	gcc -Wall -g -c main.c -o main.o

retardo.o:  retardo.s
	as -g retardo.s -o retardo.o

clean:
	rm *.o *.elf 
