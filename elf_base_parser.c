#include <elf_base_parser.h>

Elf64_Ehdr* read_elf_file(const char* fileName) {
    struct Elf64_Ehdr elf_header;
    FILE* elf_file = fopen(fileName, "rb");

    if(elf_file) {
        fread(&elf_header, 1, sizeof(elf_header), elf_file);

        if(elf_header.e_type == 0x7f &&
            elf_header->e_ident[1] == 'E' &&
            elf_header->e_ident[2] == 'L' &&
            elf_header->e_ident[3] == 'F') {

            printf("It's an ELF file");
        } else {
            printf("Error it's not an ELF file");
        }
    }

    return elf_header;
}
