#include "elf_base_parser.h"


int check_elf_file(const Elf64_Ehdr* eh) {
  
  if (eh == NULL) {
    printf("Error: ELF header is NULL\n");
    return 1;
  }
  
  
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


int get_basic_info(const char* elf_file) 
{
  
  if (elf_file == NULL) {
    printf("Error: ELF file is NULL\n");
    return EXIT_FAILURE;
  }
  
  struct stat fileStat;
  
  if (stat(elf_file, &fileStat) == -1)
    {
        perror("stat");
        return EXIT_FAILURE;
    }

  mode_t filePermissions = fileStat.st_mode & 0777;
  int blocksNbr = fileStat.st_blocks;
  size_t fileSize = fileStat.st_size;
  int inodeNbr = fileStat.st_ino;
  int owner_uid = fileStat.st_uid;
  size_t blockSize = fileStat.st_blksize;
  size_t blocksSize = blockSize * blocksNbr;
  

  struct passwd *pwd = getpwuid(owner_uid);

  if (pwd == NULL) {
    perror("getpwuid");
    return EXIT_FAILURE;
  }
  printf("\n==== Basic Informations about the binary ====\n\n");
  printf("File permissions = %o\n", filePermissions);
  printf("Number of blocks = %d\n", blocksNbr);
  printf("Blocks size = %zu bytes\n", blocksSize);
  printf("File size = %zu bytes\n", fileSize);
  printf("Inode number = %d\n", inodeNbr);
  printf("Owner UID = %d and Owner name = %s\n\n", owner_uid, pwd->pw_name);

  return 0;

}

int get_entrypoint_adress(const Elf64_Ehdr* eh) {

  if (eh == NULL) {
    printf("Error: ELF header is NULL\n");
    return EXIT_FAILURE;
  }
  
  printf("The entry point address is : 0x%x\n", eh->e_entry);
  return 0;
}

Elf64_Shdr* elf_section_header(FILE* file, const Elf64_Ehdr* eh) {
  fseek(file, eh->e_shoff, SEEK_SET);
  Elf64_Shdr* sh = malloc(eh->e_shentsize * eh->e_shnum);
  fread(sh, eh->e_shentsize, eh->e_shnum, file);

  return sh;
}

Section find_text_section(FILE* file, const Elf64_Ehdr* eh, Elf64_Shdr* sh) {
  Section special_section = { 0 };
  
  Elf64_Shdr* str_section_header = &sh[eh->e_shstrndx];
  void* str_table = malloc(str_section_header->sh_size);
  fseek(file, str_section_header->sh_offset, SEEK_SET);
  fread(str_table, 1, str_section_header->sh_size, file);

  for(int i = 0; i < eh->e_shnum; i++) {
    char* section_name = (char*)str_table + sh[i].sh_name;
    if(strcmp(section_name, ".text") == 0) {
      special_section.addr = sh[i].sh_addr;
      special_section.offset = sh[i].sh_offset;
      special_section.size = sh[i].sh_size;

    }
  }

  free(str_table);
 
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
  printf("==== Hexadecimal Content of .text section ==== \n");
  for(int i = 0; i < ts->size; i++) {
    if(i % 20 == 0) {
      printf("\n");
      printf("    ");
    }
    else if(i % 5 == 0) {
      printf(" ");
    }
    else {
    printf("%02x", ((unsigned char *)text)[i]);
    }
  }
  printf("\n");
  free(text);
}

void print_section_names(FILE* file, const Elf64_Ehdr* eh, Elf64_Shdr* sh){
  Elf64_Shdr* str_section_header = &sh[eh->e_shstrndx];
  void* str_table = malloc(str_section_header->sh_size);
  fseek(file, str_section_header->sh_offset, SEEK_SET);
  fread(str_table, 1, str_section_header->sh_size, file);

  printf("==== Sections Informations ==== \n");
  for(int i = 0; i < eh->e_shnum; i++) {
    const char* section_name = (const char*)str_table + sh[i].sh_name;
    printf("Section [%d] \n", i);
    printf("\tName : %s \n", section_name);
    printf("\tFirst byte address : % " PRIu64 "\n", sh[i].sh_addr);
    printf("\tSize : % " PRIu64 "\n", sh[i].sh_size);
  }
  printf("\n");

  free(str_table);
}

void print_linked_librairies(FILE* file, const Elf64_Ehdr* eh, Elf64_Shdr* sh, const Section dynamic_section) {
  Elf64_Dyn* current_dyn = dynamic_section.dyn;
  Section str_dyn_section = find_str_dyn_section(file, eh, sh);
  void* str_table = malloc(str_dyn_section.size);
  fseek(file, str_dyn_section.offset, SEEK_SET);
  fread(str_table, 1, str_dyn_section.size, file);

  printf("==== Dynamic Linking Librairies ====\n");
  while(current_dyn->d_tag != DT_NULL) {
    if(current_dyn->d_tag == DT_NEEDED) {
      const char* lib_name = (const char*)str_table + current_dyn->d_un.d_val;
      printf(" - %s\n", lib_name);
    }
    current_dyn = (Elf64_Dyn*)((uint8_t*)current_dyn + dynamic_section.size);
  }

  free(str_table);
}

// Factoriser les fonctions find_section_* pour en avoir plus qu'une.
// Chaques fonctions qui renvoie une section devra avoir son resultat stocké dans main pour free().
// Améliorer la présentation des informations.
