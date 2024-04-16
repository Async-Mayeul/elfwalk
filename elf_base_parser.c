#include "elf_base_parser.h"

int check_elf_file(const Elf64_Ehdr* eh) {
  if(eh->e_ident[0] == 0x7f &&
    eh->e_ident[1] == 'E' &&
    eh->e_ident[2] == 'L' &&
    eh->e_ident[3] == 'F') {

      printf("It's an ELF file");
      return 0;
  } else {
      printf("Error it's not an ELF file");
      return 1;
  }
}
