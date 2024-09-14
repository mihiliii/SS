#pragma once

#include <cstdint>

typedef uint32_t Elf32_Addr;
typedef uint32_t Elf32_Off;
typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Word;
typedef int32_t Elf32_SWord;
typedef uint8_t Elf32_Byte;
typedef int8_t Elf32_SByte;

#define ELF_WRITE 0x0  // Object is used for writing
#define ELF_READ  0x1  // Object is used for reading

// ****************************************************************************
// *                                ELF HEADER                                *
// ****************************************************************************

#define EI_NIDENT 16 /* ELF identification size */

// Makros used e_type in Elf32_Ehdr (ELF header)
#define ET_NONE 0 /* No file type */
#define ET_REL  1 /* Relocatable file */
#define ET_EXEC 2 /* Executable file */
#define ET_DYN  3 /* Shared object file */

// Struct that represents the ELF header
struct Elf32_Ehdr {
    Elf32_Half e_type;       // File type
    Elf32_Addr e_entry;      // Entry point address (virtual address where the program starts)
    Elf32_Off e_phoff;       // Program header table file offset
    Elf32_Off e_shoff;       // Section header table file offset
    Elf32_Half e_phentsize;  // Program header table entry size
    Elf32_Half e_phnum;      // Program header table entry count
    Elf32_Half e_shentsize;  // Section header table entry size
    Elf32_Half e_shnum;      // Section header table entry count
    Elf32_Off e_stroff;      // String table file offset

    Elf32_Ehdr()
        : e_type(ET_NONE),
          e_entry(0),
          e_phoff(0),
          e_shoff(0),
          e_phentsize(0),
          e_phnum(0),
          e_shentsize(0),
          e_shnum(0),
          e_stroff(0) {}
};

// ****************************************************************************
// *                           SECTION HEADER TABLE                           *
// ****************************************************************************

// Indexes of special sections
#define SHN_UNDEF 0xfff0 /* Undefined section */
#define SHN_ABS   0xfff1 /* Associated symbol is absolute */

// Makros used in sh_type in Elf32_Shdr (Section header table entry)
#define SHT_NULL     0x0 /* Unused section */
#define SHT_SYMTAB   0x1 /* Symbol table */
#define SHT_STRTAB   0x2 /* Section header string table */
#define SHT_CUSTOM   0x3 /* Custom section */
#define SHT_RELA     0x4 /* Relocation entries with addends */
#define SHT_PROGBITS 0x5 /* Program data */

// Struct that represents the section table entry
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
    Elf32_Word st_name;   // Offset in the symbol string name table that matches that symbol name
    Elf32_Byte st_info;   // Type attributes
    Elf32_Half st_shndx;  // Section index which the symbol is defined
    Elf32_Addr st_value;  // Symbol value
    Elf32_Word st_size;   // Size of the symbol
    bool st_defined;      // True if the symbol is defined in the ELF file
};

// Makros used in r_info in Elf32_Rela (Relocation table entry)
#define ELF32_R_INFO(t, s) (((t) << 8) + ((s) & 0xff)) /* Symbol table index and relocation type */
#define ELF32_R_TYPE(i)    ((i) >> 8)                  /* Relocation type */
#define ELF32_R_SYM(i)     ((i) & 0xff)                /* Symbol table index */
#define ELF32_R_ABS32      0x0                         /* Absolute relocation */

// Struct that represents the relocation table entry
struct Elf32_Rela {
    Elf32_Addr r_offset;   // Offset in the section where the relocation should be applied
    Elf32_Word r_info;     // Symbol table index that is used to determine whose value should be used in the relocation
    Elf32_SWord r_addend;  // Constant addend used to compute the value to be stored
};

// ****************************************************************************
// *                           PROGRAM HEADER TABLE                           *
// ****************************************************************************

#define PT_NULL 0 /* Unused entry */
#define PT_LOAD 1 /* Loadable segment */

// Struct that represents the program header table entry
struct Elf32_Phdr {
    Elf32_Word p_type;  // Type of the segment
    // Elf32_Word p_flags;  // Segment attributes
    Elf32_Off p_offset;   // Offset of the segment in the ELF file
    Elf32_Addr p_vaddr;   // Virtual address of the segment in memory
    Elf32_Addr p_paddr;   // Physical address of the segment in memory
    Elf32_Word p_filesz;  // Size of the segment in the ELF file
    Elf32_Word p_memsz;   // Size of the segment in memory
    Elf32_Word p_align;   // Required alignment of the segment
};