#pragma once

#include <cstdint>

typedef uint32_t Elf32_Addr;
typedef uint32_t Elf32_Off;
typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Word;
typedef int32_t Elf32_SWord;

// ****************************************************************************
// *                                ELF HEADER                                *
// ****************************************************************************

// Makros used in e_ident array in Elf32_Ehdr (ELF header)
#define EI_NIDENT 8 /* ELF identification size */

// Makros used e_type in Elf32_Ehdr (ELF header)
#define ET_NONE 0 /* No file type */
#define ET_REL  1 /* Relocatable file */
#define ET_EXEC 2 /* Executable file */
#define ET_DYN  3 /* Shared object file */

// Struct that represents the ELF header
struct Elf32_Ehdr {
    unsigned char e_identification[EI_NIDENT] = {'E', 'L', 'F', 'H', 'E', 'A', 'D', 'F'};  // ELF identification
    Elf32_Half e_type;                                                                     // File type
    Elf32_Addr e_entry;      // Entry point address (virtual address where the program starts)
    Elf32_Off e_phoff;       // Program header table file offset
    Elf32_Off e_shoff;       // Section header table file offset
    Elf32_Half e_phentsize;  // Program header table entry size
    Elf32_Half e_phnum;      // Program header table entry count
    Elf32_Half e_shentsize;  // Section header table entry size
    Elf32_Half e_shnum;      // Section header table entry count
    Elf32_Half e_shstrndx;   // Section header string table index entry
};

// ****************************************************************************
// *                           SECTION HEADER TABLE                           *
// ****************************************************************************

// #define SHN_UNDEF     0      /* Undefined section */
// #define SHN_LORESERVE 0xff00 /* Start of reserved indices */
// #define SHN_LOPROC    0xff00 /* Start of processor-specific */
// #define SHN_HIPROC    0xff1f /* End of processor-specific */
// #define SHN_LOOS      0xff20 /* Start of OS-specific */
// #define SHN_HIOS      0xff3f /* End of OS-specific */
// #define SHN_ABS       0xfff1 /* Associated symbol is absolute */
// #define SHN_COMMON    0xfff2 /* Associated symbol is common */
// #define SHN_XINDEX    0xffff /* Index is in extra table. */
// #define SHN_HIRESERVE 0xffff /* End of reserved indices */

#define SHT_SYMTAB 0x1 /* Symbol table */
#define SHT_STRTAB 0x2 /* Section header string table */
#define SHT_CUSTOM 0x3 /* Custom section */
#define SHT_RELA   0x4 /* Relocation entries with addends */

// Struct that represents the section table entry
struct Elf32_Shdr {
    Elf32_Word sh_name;       // Section name (string table index)
    Elf32_Word sh_type;       // Section type, not used
    Elf32_Word sh_flags;      // Section attributes, not used
    Elf32_Addr sh_addr;       // Virtual address in memory
    Elf32_Off sh_offset;      // Offset of the section from start of ELF file
    Elf32_Word sh_size;       // Size of the section in bytes
    Elf32_Word sh_link;       // Index of a related section (if any)
    Elf32_Word sh_info;       // Extra information about the section
    Elf32_Word sh_addralign;  // Required alignment of the section
    Elf32_Word sh_entsize;    // Size of each entry in the section
};

// ****************************************************************************
// *                               SYMBOL TABLE                               *
// ****************************************************************************

// Makros used in st_info in Elf32_Sym (Symbol table entry)
#define ELF32_ST_BIND(i)    ((i) >> 4)                 /* Symbol binding */
#define ELF32_ST_TYPE(i)    ((i) & 0xf)                /* Symbol type */
#define ELF32_ST_INFO(b, t) (((b) << 4) + ((t) & 0xf)) /* Symbol type and binding */

// Symbol table bindings
#define STB_LOCAL  0 /* Local symbol */
#define STB_GLOBAL 1 /* Global symbol */
#define STB_WEAK   2 /* Weak symbol */

// Symbol table types
#define STT_NOTYPE  0 /* No type */
#define STT_SECTION 1 /* Section */
#define STT_FILE    2 /* File */

// Symbol table visibility
#define STV_DEFAULT 0 /* Default visibility */

// Struct that represents the symbol table entry
struct Elf32_Sym {
    Elf32_Word st_name;      // Offset in the symbol string name table that matches that symbol name
    unsigned char st_info;   // Type attributes
    unsigned char st_other;  // Symbol visibility
    Elf32_Half st_shndx;     // Section index which the symbol is defined
    Elf32_Addr st_value;     // Symbol value
    Elf32_Word st_size;      // Size of the symbol
    bool st_defined;         // True if the symbol is defined in the ELF file
};

// Struct that represents the relocation table entry
struct Elf32_Rela {
    Elf32_Addr r_offset;   // Offset in the section where the relocation should be applied
    Elf32_Word r_info;     // Symbol table index that is used to determine whose value should be used in the relocation
    Elf32_SWord r_addend;  // Constant addend used to compute the value to be stored
};
