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

Section find_text_section(FILE* file, const Elf64_Ehdr* eh, const Elf64_Shdr* sh) {
  Section special_section = { 0 };
  
  for(int i = 0; i < eh->e_shnum; i++) {
    if(sh[i].sh_type == SHT_PROGBITS && sh[i].sh_flags & SHF_EXECINSTR) {
      if(sh[i].sh_size == 0) {
        printf("No .text section\n");
      }
      special_section.addr = sh[i].sh_addr;
      special_section.offset = sh[i].sh_offset;
      special_section.size = sh[i].sh_size;
      break;
    }
  }
  
  return special_section;
}

Section find_str_dyn_section(FILE* file, const Elf64_Ehdr* eh, const Elf64_Shdr* sh) {
  Section special_section = { 0 };
  
  for(int i = 0; i < eh->e_shnum; i++) {
    if(sh[i].sh_type == SHT_STRTAB && sh[i].sh_flags & SHF_ALLOC) {
      if(sh[i].sh_size == 0) {
        printf("No .dynstr section\n");
      }
      special_section.addr = sh[i].sh_addr;
      special_section.offset = sh[i].sh_offset;
      special_section.size = sh[i].sh_size;
      break;
    }
  }
  
  return special_section;
}

Section find_dynamic_section(FILE* file, const Elf64_Ehdr* eh, const Elf64_Shdr* sh) {
  Section special_section = { 0 };

  for(int i = 0; i < eh->e_shnum; i++) {
    if(sh[i].sh_type == SHT_DYNAMIC) {
      special_section.dyn = malloc(sh[i].sh_size);
      fseek(file, sh[i].sh_offset, SEEK_SET);
      fread(special_section.dyn, 1, sh[i].sh_size, file);
      special_section.size = sh[i].sh_size;
      break;
    }
  }

  return special_section;
}

void print_total_sections(const Elf64_Ehdr* eh) {
  printf("Total number of sections : %" PRIu16 "\n", eh->e_shnum);
}

void print_text_section(FILE* file, const Section* ts) {
  void* text = malloc(ts->size);
  fseek(file, ts->offset, SEEK_SET);
  fread(text, 1, ts->size, file);
  for(int i = 0; i < ts->size; i++) {
    printf("%02x\n", ((unsigned char *)text)[i]);
  }
  free(text);
}

void print_section_names(FILE* file, const Elf64_Ehdr* eh, Elf64_Shdr* sh){
  Elf64_Shdr* str_section_header = &sh[eh->e_shstrndx];
  void* str_table = malloc(str_section_header->sh_size);
  fseek(file, str_section_header->sh_offset, SEEK_SET);
  fread(str_table, 1, str_section_header->sh_size, file);

  printf("Section names: \n");
  for(int i = 0; i < eh->e_shnum; i++) {
    const char* section_name = (const char*)str_table + sh[i].sh_name;
    printf("%d : %s\n", i, section_name);
  }

  free(str_table);
}

void print_linked_librairies(FILE* file, const Elf64_Ehdr* eh, Elf64_Shdr* sh, const Section dynamic_section) {
  Elf64_Dyn* current_dyn = dynamic_section.dyn;
  Section str_dyn_section = find_str_dyn_section(file, eh, sh);
  void* str_table = malloc(str_dyn_section.size);
  fseek(file, str_dyn_section.offset, SEEK_SET);
  fread(str_table, 1, str_dyn_section.size, file);

  

  while(current_dyn->d_tag != DT_NULL) {
    if(current_dyn->d_tag == DT_STRTAB) {
      printf("string table\n");
    }
    if(current_dyn->d_tag == DT_NEEDED) {
      const char* lib_name = (const char*)str_table + current_dyn->d_un.d_val;
      printf(" - %s\n", lib_name);
    }
    current_dyn = (Elf64_Dyn*)((uint8_t*)current_dyn + dynamic_section.size);
  }

  free(str_table);
}

// TODO : 
// Factoriser les fonctions find_section_* pour en avoir plus qu'une.
// Chaques fonctions qui renvoie une section devra avoir son resultat stocké dans main pour free().
// Améliorer la présentation des informations.
