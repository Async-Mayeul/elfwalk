#include <stdio.h>
#include <stdlib.h>
#include <elf.h>

#ifndef SIMPLE_ELF
#define SIMPLE_ELF

unsigned int get_file_size(Elf32_Ehdr eh);
unsigned char get_file_rights(Elf32_Ehdr eh);
const char* get_owner_info(Elf32_Ehdr eh);
unsigned int get_number_of_blocks(Elf32_Ehdr eh);
unsigned int get_blocks_size(Elf32_Ehdr eh);
unsigned int get_inode(Elf32_Ehdr eh);

#endif
