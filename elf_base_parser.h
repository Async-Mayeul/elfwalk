#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#ifndef SIMPLE_ELF
#define SIMPLE_ELF

typedef struct {
    uint8_t     e_ident[16];         /* Magic number and other info */
    uint16_t    e_type;              /* Object file type */
    uint16_t    e_machine;           /* Architecture */
    uint32_t    e_version;           /* Object file version */
    uint64_t    e_entry;             /* Entry point virtual address */
    uint64_t    e_phoff;             /* Program header table file offset */
    uint64_t    e_shoff;             /* Section header table file offset */
    uint32_t    e_flags;             /* Processor-specific flags */
    uint16_t    e_ehsize;            /* ELF header size in bytes */
    uint16_t    e_phentsize;         /* Program header table entry size */
    uint16_t    e_phnum;             /* Program header table entry count */
    uint16_t    e_shentsize;         /* Section header table entry size */
    uint16_t    e_shnum;             /* Section header table entry count */
    uint16_t    e_shstrndx;          /* Section header string table index */
} Elf64_Ehdr;


int check_elf_file(const Elf64_Ehdr* eh);
//unsigned int get_file_size(const Elf64_Ehdr* eh);
//unsigned char get_file_rights(const Elf64_Ehdr* eh);
//const char* get_owner_info(const Elf64_Ehdr* eh);
//unsigned int get_number_of_blocks(const Elf64_Ehdr* eh);
//unsigned int get_blocks_size(const Elf64_Ehdr* eh);
//unsigned int get_inode(const Elf64_Ehdr* eh);

#endif
