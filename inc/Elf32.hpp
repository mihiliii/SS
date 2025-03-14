#pragma once

#include <cstdint>

typedef uint32_t Elf32_Addr;
typedef uint32_t Elf32_Off;
typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Word;
typedef int32_t Elf32_SWord;
typedef uint8_t Elf32_Byte;
typedef int8_t Elf32_SByte;

// ****************************************************************************
// *                                ELF HEADER                                *
// ****************************************************************************

/* Makros used e_type in Elf32_Ehdr (ELF header) */

#define ET_NONE 0  // No file type
#define ET_REL  1  // Relocatable file
#define ET_EXEC 2  // Executable file

#define EI_IDENTIFIER \
    'C', 'U', 'S', 'T', 'O', 'M', ' ', 'E', 'L', 'F', ' ', 'F', 'I', 'L', 'E', '\0'

/* Struct that represents the ELF header */
struct Elf32_Ehdr {
    Elf32_Byte e_ident[16];  // ELF identification
    Elf32_Half e_type;       // File type
    Elf32_Addr e_entry;      // Entry point address (virtual address where the program starts)
    Elf32_Off e_shoff;       // Section header table file offset
    Elf32_Half e_shentsize;  // Section header table entry size
    Elf32_Half e_shnum;      // Section header table entry count
    Elf32_Off e_stroff;      // String table file offset
};

// ****************************************************************************
// *                           SECTION HEADER TABLE                           *
// ****************************************************************************

/* Indexes of special sections */

#define SHN_ABS 0xfff1  // Associated symbol is absolute

/* Makros used in sh_type in Elf32_Shdr (Section header table entry) */

#define SHT_NULL   0x0  // Unused section
#define SHT_SYMTAB 0x1  // Symbol table
#define SHT_STRTAB 0x2  // Section header string table
#define SHT_CUSTOM 0x3  // Custom section
#define SHT_RELA   0x4  // Relocation entries with addends

/* Struct that represents the section table entry */
struct Elf32_Shdr {
    Elf32_Word sh_name;       // Section name (string table index)
    Elf32_Word sh_type;       // Section type
    Elf32_Addr sh_addr;       // Virtual address in memory
    Elf32_Off sh_offset;      // Offset of the section from start of ELF file
    Elf32_Word sh_size;       // Size of the section in bytes
    Elf32_Word sh_link;       // Index of a related section (if any)
    Elf32_Word sh_info;       // Extra information about the section
    Elf32_Word sh_addralign;  // Required alignment of the section
    Elf32_Word sh_entsize;    // Size of each entry in the section
};
// TODO: remove sh_info and put every info needed in sh_link

// ****************************************************************************
// *                               SYMBOL TABLE                               *
// ****************************************************************************

/* Makros used in st_info in Elf32_Sym (Symbol table entry) */

#define ELF32_ST_BIND(i)    ((i) >> 4)                  // Gets symbol binding from st_info
#define ELF32_ST_TYPE(i)    ((i) & 0xf)                 // Gets symbol type from st_info
#define ELF32_ST_INFO(b, t) (((b) << 4) + ((t) & 0xf))  // Creates st_info from binding and type

/* Symbol table bindings */

#define STB_LOCAL  0  // Local symbol
#define STB_GLOBAL 1  // Global symbol
#define STB_WEAK   2  // Weak symbol

/* Symbol table types */

#define STT_NOTYPE  0  // No type
#define STT_SECTION 1  // Section

/* Struct that represents the symbol table entry */
struct Elf32_Sym {
    Elf32_Word st_name;   // Offset in the symbol string name table that matches that symbol name
    Elf32_Addr st_value;  // Symbol value
    Elf32_Word st_size;   // Size of the symbol
    Elf32_Half st_shndx;  // Section index which the symbol is defined
    Elf32_Byte st_info;   // Type attributes
    bool st_defined;      // True if the symbol is defined in the ELF file
};

// ****************************************************************************
// *                             RELOCATION TABLE                             *
// ****************************************************************************

/* Makros used in r_info in Elf32_Rela (Relocation table entry) */

#define ELF32_R_TYPE(i)    ((i) >> 8)                   // Gets relocation type from r_info
#define ELF32_R_SYM(i)     ((i) & 0xff)                 // Gets symbol index from r_info
#define ELF32_R_INFO(t, s) (((t) << 8) + ((s) & 0xff))  // Creates r_info from type and symbol

#define ELF32_R_TYPE_ABS32 0x0  // Absolute relocation

/* Struct that represents the relocation table entry */
struct Elf32_Rela {
    Elf32_Addr r_offset;  // Offset in the section where the relocation should be applied
    Elf32_Word r_info;    // Symbol table index that is used to determine whose value should be used
    Elf32_SWord r_addend;  // Constant addend used to compute the value to be stored
};
