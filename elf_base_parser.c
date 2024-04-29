#include "elf_base_parser.h"

int check_elf_file(const Elf64_Ehdr* eh) {
  if(eh->e_ident[0] == 0x7f &&
    eh->e_ident[1] == 'E' &&
    eh->e_ident[2] == 'L' &&
    eh->e_ident[3] == 'F') {

      printf("It's an ELF file\n");
      return 0;
  } else {
      printf("Error it's not an ELF file\n");
  return 1;
  }
}

Elf64_Shdr* elf_section_header(FILE* file, const Elf64_Ehdr* eh) {
  fseek(file, eh->e_shoff, SEEK_SET);
  Elf64_Shdr* sh = malloc(eh->e_shentsize * eh->e_shnum);
  fread(sh, eh->e_shentsize, eh->e_shnum, file);

  return sh;
}

Section find_text_section(FILE* file, const Elf64_Ehdr* eh) {
  Elf64_Shdr* sh = elf_section_header(file, eh);
  Section text_section = { 0 };
  for(int i = 0; i < eh->e_shnum; i++) {
    if(sh[i].sh_type == SHT_PROGBITS && sh[i].sh_flags & SHF_EXECINSTR) {
      if(sh[i].sh_size == 0) {
        printf("No .text section\n");
      }
      text_section.addr = sh[i].sh_addr;
      text_section.offset = sh[i].sh_offset;
      text_section.size = sh[i].sh_size;
      break;
    }
  }
    
  return text_section;
}

Section find_string_table(FILE* file, const Elf64_Ehdr* eh) {
  Elf64_Shdr* sh = elf_section_header(file, eh);
  Section str_section = { 0 };
  for(int i = 0; i < eh->e_shnum; i++) {
    if(sh[i].sh_type == SHT_STRTAB) {
      str_section.addr = sh[i].sh_addr;
      str_section.offset = sh[i].sh_offset;
      str_section.size = sh[i].sh_size;
      break;
    }
  }

  return str_section;
}

void print_total_sections(const Elf64_Ehdr* eh) {
  printf("Total number of sections : %" PRIu16 "\n", eh->e_shnum);
}

void print_text_section(FILE* file, Section* ts) {
      void* text = malloc(ts->size);
      fseek(file, ts->offset, SEEK_SET);
      fread(text, 1, ts->size, file);
      for(int i = 0; i < ts->size; i++) {
        printf("%02x\n", ((unsigned char *)text)[i]);
      }
      printf;
      free(text);
}

void print_section_names(FILE* file, Elf64_Ehdr* eh){
  Elf64_Shdr* sh = elf_section_header(file, eh);
  Section str_section = find_string_table(file, eh);
  void* str_table = malloc(str_section.size);
  fseek(file, str_section.offset, SEEK_SET);
  fread(str_table, 1, str_section.size, file);

  printf("Section names: \n");
  for(int i = 0; i < eh->e_shnum; i++) {
    const char* section_name = (const char *)str_table + sh[i].sh_name;
    printf("%d : %s\n", i, section_name);
  }

  free(str_table);
}
