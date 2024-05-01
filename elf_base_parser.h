#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include <elf.h>
#include <inttypes.h>
#include <pwd.h>
#include <getopt.h>
#include <ctype.h>

#ifndef SIMPLE_ELF
#define SIMPLE_ELF

typedef struct {
    Elf64_Dyn* dyn;
    uint64_t addr;
    uint64_t offset;
    uint64_t size;
} Section;

int check_elf_file(const Elf64_Ehdr* eh);
int get_basic_info(const char* elf_file);
int get_entrypoint_adress(const Elf64_Ehdr* eh);
Elf64_Shdr* elf_section_header(FILE* file, const Elf64_Ehdr* eh);
Section find_text_section(FILE* file, const Elf64_Ehdr* eh, Elf64_Shdr* sh);
Section find_dynamic_section(FILE* file, const Elf64_Ehdr* eh, const Elf64_Shdr* sh);
Section find_str_dyn_section(FILE* file, const Elf64_Ehdr* eh, const Elf64_Shdr* sh);
void print_linked_librairies(FILE* file, const Elf64_Ehdr* eh, Elf64_Shdr* sh, const Section dynamic_section);
void print_total_sections(const Elf64_Ehdr* eh);
void print_text_section(FILE* file, const Section* ts);
void print_section_names(FILE* file,const Elf64_Ehdr* eh, Elf64_Shdr* sh);
#endif
