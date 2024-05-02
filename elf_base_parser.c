#include "elf_base_parser.h"

/**
 * This function check if file object provided is on ELF format.
 * Args: pointer to ELF file header.
 * Ret : return 1 on error or 0 on success.
 * **/
int check_elf_file(const Elf64_Ehdr* eh) {
  
  if (eh == NULL) {
    printf("Error: ELF header is NULL\n");
    return 1;
  }
  if(eh->e_ident[0] == 0x7f &&
    eh->e_ident[1] == 'E' &&
    eh->e_ident[2] == 'L' &&
    eh->e_ident[3] == 'F') {

      printf("\n === It's an ELF file === \n");
      return 0;
  } else {
      printf("Error it's not an ELF file\n");
      return 1;
  }
}

/**
 * This function get and print basic information of ELF file
 * like permissions, Inode number, Owner...
 * Arg: file descriptor of ELF file
 * Return: return 1 on failure or 0 on success
 * **/
int get_basic_info(const char* elf_file) 
{
  struct stat fileStat;

  if (elf_file == NULL) {
    printf("Error: ELF file is NULL\n");
    return EXIT_FAILURE;
  }
  
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

/**
 * This function return the entrypoint adress ELF program
 * Arg: ELF file header
 * Return: return 1 on failure or 0 on success
 * **/
int get_entrypoint_adress(const Elf64_Ehdr* eh) {

  if (eh == NULL) {
    printf("Error: ELF header is NULL\n");
    return EXIT_FAILURE;
  }
  
  printf("The entry point address is : 0x%lx\n", eh->e_entry);
  return 0;
}

/**
 * This fonction find the elf section header
 * Args: file descriptor and ELF file header
 * Return: ELF section header
 * **/
Elf64_Shdr* elf_section_header(FILE* file, const Elf64_Ehdr* eh) {
  fseek(file, eh->e_shoff, SEEK_SET);
  uint64_t size = eh->e_shentsize * eh->e_shnum;
  Elf64_Shdr* sh = malloc(size);

  if(!sh && size) {
    printf("Error elf_section_header: cannot allocate memory\n");
    exit(-1);
  }

  fread(sh, eh->e_shentsize, eh->e_shnum, file);

  return sh;
}

/**
 * This function return .text section informations
 * Args: file descriptor, ELF file header and ELF section header
 * Return: struct that contains offset, size and adress of .text section
 * **/
Section find_text_section(FILE* file, const Elf64_Ehdr* eh, Elf64_Shdr* sh) {
  Section special_section = { 0 };
  
  Elf64_Shdr* str_section_header = &sh[eh->e_shstrndx];
  void* str_table = malloc(str_section_header->sh_size);

  if(!str_table && str_section_header->sh_size) {
    printf("Error find_text_section: cannont allocate memory\n");
    exit(-1);
  }

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

/**
 * This function return informations of .strdyn section
 * Args: file descriptor, ELF file header and ELF section header
 * Return: struct that contain offset, size and adress of .strdyn section
 * **/
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
      if(!special_section.dyn && sh[i].sh_size) {
        printf("Error find_dynamic_section: cannot allocate memory\n");
        exit(-1);
      }
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

/**
 * This function print the instructions of an elf file in hex format
 * Args: file descriptor and struct that contains informations of .text section
 * **/
void print_text_section(FILE* file, const Section* ts) {
  void* text = malloc(ts->size);

  if(!text && ts->size) {
    printf("Error print_text_section: cannot allocate memory\n");
    exit(-1);
  }

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

/**
 * This function print the names of each sections
 * Args: file descriptor, ELF file header and ELF section header
 * **/
void print_section_names(FILE* file, const Elf64_Ehdr* eh, Elf64_Shdr* sh){
  Elf64_Shdr* str_section_header = &sh[eh->e_shstrndx];
  void* str_table = malloc(str_section_header->sh_size);

  if(!str_table && str_section_header->sh_size) {
    printf("Error print_section_names: cannot allocate memory\n");
    exit(-1);
  }

  fseek(file, str_section_header->sh_offset, SEEK_SET);
  fread(str_table, 1, str_section_header->sh_size, file);

  printf("==== Sections Informations ==== \n");

  for(int i = 0; i < eh->e_shnum; i++) {
    const char* section_name = (const char*)str_table + sh[i].sh_name;
    printf("Section [%d] \n", i);
    printf("\tName : %s \n", section_name);
    printf("\tFirst byte address : %" PRIu64 "\n", sh[i].sh_addr);
    printf("\tSize : %" PRIu64 "\n", sh[i].sh_size);
  }
  printf("\n");

  free(str_table);
}

/**
 * This function print the name of linked librairies
 * Args: file descriptor, ELF file header, ELF section header and struct of dynamic section
 * **/
void print_linked_librairies(FILE* file, const Elf64_Ehdr* eh, Elf64_Shdr* sh, const Section dynamic_section) {
  Elf64_Dyn* current_dyn = dynamic_section.dyn;
  Section str_dyn_section = find_str_dyn_section(file, eh, sh);

  void* str_table = malloc(str_dyn_section.size);

  if(!str_table && str_dyn_section.size) {
    printf("Error print_linked_librairies: cannot allocate memory\n");
    exit(-1);
  }

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
/**
 * This function print capabilities of filesystme on which is the ELF file
 * Args: file descriptor
 * **/
int print_fs_capabilities(const char* elf_file, FILE* file)
{
  struct statfs st;

  int fd = fileno(file);

  if (fstatfs(fd, &st) == -1)
  {
    perror("fstatfs");
    return EXIT_FAILURE;
  }
  
  printf("\n==== Filesystem Informations ====\n\n");

  if (st.f_type == 0x58465342) {
    printf("Filesystem type: XFS\n");
  }
  else if (st.f_type == 0xef53) {
    printf("Filesystem type: EXT4\n");
  }
  else if (st.f_type == 0x1021994) {
    printf("Filesystem type: BTRFS\n");
  }
  else {
    printf("Filesystem type: Unknown\n");
  }
  
  printf("Total data blocks in filesystem: %ld\n", st.f_blocks);
  printf("Optimal transfer block size: %ld\n", st.f_bsize);
  printf("Free blocks in filesystem : %ld\n", st.f_bfree);
  printf("Free blocks available to unprivileged user: %ld\n", st.f_bavail);
  printf("Total inodes in filesystem: %ld\n", st.f_files);
  printf("Free inodes in filesystem: %ld\n", st.f_ffree);
  
  return 0;

}

/**
 * This function print string in string table
 * Args: file descriptor, ELF file header and ELF section header
 * **/
int print_data_in_string_table(FILE* file, Elf64_Ehdr* eh, Elf64_Shdr* sh) {
  Elf64_Shdr* str_section_header = &sh[eh->e_shstrndx];
  char* str_table = malloc(str_section_header->sh_size);

  if(!str_table && str_section_header->sh_size) {
    printf("Error print_data_in_string_table: cannot allocate memory\n");
    exit(-1);
  }

  fseek(file, str_section_header->sh_offset, SEEK_SET);
  fread(str_table, 1, str_section_header->sh_size, file);

  for (int i = 0; i < str_section_header->sh_size; i++)
  {
    const char str = str_table[i];
    printf("%c", str);
  }
  
  free(str_table);

  return 0;
}