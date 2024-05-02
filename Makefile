CC = gcc

main: main.o elf_base_parser.o
	$(CC) -g -o $@ $^

main.o: main.c elf_base_parser.h 
	$(CC) -c -Wall $<

elf_base_parser.o: elf_base_parser.c elf_base_parser.h 
	$(CC) -c -Wall $<

clean:
	rm -f main *.o
