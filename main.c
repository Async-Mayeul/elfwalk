#include "elf_base_parser.h"


int main(int argc, char *argv[]) {
    int c;
    int print_text = 0, print_sections = 0, print_section_info = 0,
        print_entrypoint = 0, print_string_table = 0, print_linked_libraries = 0, print_fs = 0;

    Elf64_Ehdr  elf_header;

    while ((c = getopt(argc, argv, "dsaetcl")) != -1) {
        switch (c) {
            case 'd':
                print_text = 1;
                break;
            case 's':
                print_sections = 1;
                break;
            case 'a':
                print_section_info = 1;
                break;
            case 'e':
                print_entrypoint = 1;
                break;
            case 't':
                print_string_table = 1;
                break;
            case 'l':
                print_linked_libraries = 1;
                break;
            case 'c':
                print_fs = 1;
                break;    
            case '?':
                if (optopt == 'd' || optopt == 's' || optopt == 'a' || optopt == 'e' || optopt == 't' || optopt == 'l') {
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                } else if (isprint(optopt)) {
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                } else {
                    fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
                }
                return EXIT_FAILURE;
            default:
                abort();
        }
    }

    
    if (optind >= argc) {
        fprintf(stderr, "Error: ELF file is missing.\n");
        return EXIT_FAILURE;
    }

    const char *elf_file = argv[optind];

    FILE* file = fopen(elf_file, "rb");
    
    if (file == NULL) {
        perror("fopen");
        return EXIT_FAILURE;
    }

    
    if(elf_file) {
      fread(&elf_header, 1, sizeof(elf_header), file);
   }

    Elf64_Shdr* section_header = elf_section_header(file, &elf_header);
    Section text_section = find_text_section(file, &elf_header, section_header);
    Section dynamic_section = find_dynamic_section(file, &elf_header, section_header);

    if (print_sections) {
        print_total_sections(&elf_header);
    }
    if (print_text) {
        print_text_section(file, &text_section);
    }
    if (print_section_info) {
        print_section_names(file, &elf_header, section_header);
    }
    if (print_linked_libraries) {
        print_linked_librairies(file, &elf_header, section_header, dynamic_section);
    }
    if (print_entrypoint) {
        get_entrypoint_adress(&elf_header);
    }
    if (print_string_table) {
        print_data_in_string_table(file, &elf_header, section_header);
    }
    if (print_fs){
        print_fs_capabilities(elf_file, file);
    }
    
    if (!print_text && !print_sections && !print_section_info && !print_entrypoint && !print_string_table && !print_linked_libraries && !print_fs) {
      check_elf_file(&elf_header);
      get_basic_info(elf_file);
      
        
    }

    free(section_header);
    free(dynamic_section.dyn);
    fclose(file);

    return 0;
}


