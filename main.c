#include "elf_base_parser.h"

int main() {
  char*       elf_file = "./test";
  Elf64_Ehdr  elf_header;
  Elf64_Shdr* section_header;
  Section     text_section;
  Section     dynamic_section;
  Section     string_section;

  FILE* file = fopen(elf_file, "rb");
  if(elf_file) {
    fread(&elf_header, 1, sizeof(elf_header), file);
  }

  section_header  = elf_section_header(file, &elf_header);
  text_section    = find_text_section(file, &elf_header, section_header);
  dynamic_section = find_dynamic_section(file, &elf_header, section_header);

  check_elf_file(&elf_header);
  print_total_sections(&elf_header);
  print_text_section(file, &text_section);
  print_section_names(file, &elf_header, section_header);
  print_linked_librairies(file, &elf_header, section_header, dynamic_section);
  
  fclose(file);

  return 0;
}
