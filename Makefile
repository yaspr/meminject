CC=gcc
CFLAGS=-Wall -Wextra -g
OFLAGS=-O1

all: clean meminject target shellcode

meminject: meminject.c
	$(CC) $(CFLAGS) $(OFLAGS) $< -o $@

target: target.c
	$(CC) $(CFLAGS) $(OFLAGS) $< -o $@

shellcode: shellcode.asm
	nasm -f elf64 -o $@.o $<
	ld -o $@ $@.o
	objdump -d $@

clean:
	rm -Rf *.o meminject target shellcode
