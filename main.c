#include "elf_base_parser.h"

int main() {
  char* elf_file = "./test";
  Elf64_Ehdr elf_header;

  FILE* file = fopen(elf_file, "rb");
  if(elf_file) {
    fread(&elf_header, 1, sizeof(elf_header), file);
  }

  check_elf_file(&elf_header);

  return 0;
}
