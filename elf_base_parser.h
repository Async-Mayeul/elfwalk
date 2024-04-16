#include <stdio.h>
#include <stdlib.h>
#include <elf.h>

#ifndef SIMPLE_ELF
#define SIMPLE_ELF

typedef struct {
    uint8     e_ident[16];         /* Magic number and other info */
    uint16    e_type;              /* Object file type */
    uint16    e_machine;           /* Architecture */
    uint32    e_version;           /* Object file version */
    uint64    e_entry;             /* Entry point virtual address */
    uint64    e_phoff;             /* Program header table file offset */
    uint64    e_shoff;             /* Section header table file offset */
    uint32    e_flags;             /* Processor-specific flags */
    uint16    e_ehsize;            /* ELF header size in bytes */
    uint16    e_phentsize;         /* Program header table entry size */
    uint16    e_phnum;             /* Program header table entry count */
    uint16    e_shentsize;         /* Section header table entry size */
    uint16    e_shnum;             /* Section header table entry count */
    uint16    e_shstrndx;          /* Section header string table index */
} Elf64_Ehdr;

Elf64_Ehdr* read_elf_file(const char* fileName);
unsigned int get_file_size(const Elf64_Ehdr* eh);
unsigned char get_file_rights(const Elf64_Ehdr* eh);
const char* get_owner_info(const Elf64_Ehdr* eh);
unsigned int get_number_of_blocks(const Elf64_Ehdr* eh);
unsigned int get_blocks_size(const Elf64_Ehdr* eh);
unsigned int get_inode(const Elf64_Ehdr* eh);

#endif
