#ifndef Elf32_hpp_
#define Elf32_hpp_

#include "Types.hpp"

typedef uint32_t Elf32_Addr;
typedef uint32_t Elf32_Off;
typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Word;
typedef int32_t  Elf32_SWord;


// ****************************************************************************
// *                                ELF HEADER                                *
// ****************************************************************************

// Makros used e_ident array in Elf32_Ehdr (ELF header)
#define EI_NIDENT   8 /* ELF identification size */
#define EI_MAG0     0 /* File identification byte 0 index */
#define EI_MAG1     1 /* File identification byte 1 index */
#define EI_MAG2     2 /* File identification byte 2 index */
#define EI_MAG3     3 /* File identification byte 3 index */
#define EI_CLASS    4 /* File class byte index */
#define EI_DATA     5 /* Data encoding byte index */
#define EI_VERSION  6 /* File version byte index */
#define EI_OSABI    7 /* OS ABI identification */
#define EI_PAD      8 /* Byte index of padding bytes */

#define ELFCLASS32  1 /* 32-bit objects */
#define ELFCLASS64  2 /* 64-bit objects */

#define ELFDATA2LSB 1 /* Little-endian */
#define ELFDATA2MSB 2 /* Big-endian */

#define EV_NONE     0 /* Invalid version */
#define EV_CURRENT  1 /* Current version */


// Makros used e_type in Elf32_Ehdr (ELF header)
#define ET_NONE 0 /* No file type */
#define ET_REL  1 /* Relocatable file */
#define ET_EXEC 2 /* Executable file */
#define ET_DYN  3 /* Shared object file */


// Struct that represents the ELF header
struct Elf32_Ehdr {
    unsigned char e_ident[EI_NIDENT] {0x7f, 'E', 'L', 'F', ELFCLASS32, ELFDATA2LSB, EV_CURRENT, 0};
    Elf32_Half    e_type;       // File type
    Elf32_Addr    e_entry;      // Entry point address
    Elf32_Off     e_phoff;      // Program header table file offset
    Elf32_Off     e_shoff;      // Section header table file offset
    Elf32_Half    e_ehsize;     // ELF header size in bytes
    Elf32_Half    e_phentsize;  // Program header table entry size
    Elf32_Half    e_phnum;      // Program header table entry count
    Elf32_Half    e_shentsize;  // Section header table entry size
    Elf32_Half    e_shnum;      // Section header table entry count
    Elf32_Half    e_shstrndx;   // Section header string table index entry
};


// ****************************************************************************
// *                           SECTION HEADER TABLE                           *
// ****************************************************************************


#define SHN_UNDEF     0      /* Undefined section */
#define SHN_LORESERVE 0xff00 /* Start of reserved indices */
#define SHN_LOPROC    0xff00 /* Start of processor-specific */
#define SHN_HIPROC    0xff1f /* End of processor-specific */
#define SHN_LOOS      0xff20 /* Start of OS-specific */
#define SHN_HIOS      0xff3f /* End of OS-specific */
#define SHN_ABS       0xfff1 /* Associated symbol is absolute */
#define SHN_COMMON    0xfff2 /* Associated symbol is common */
#define SHN_XINDEX    0xffff /* Index is in extra table. */
#define SHN_HIRESERVE 0xffff /* End of reserved indices */


// Struct that represents the symbol table entry
typedef struct {
    Elf32_Word sh_name;   // Section name (string table index)
    Elf32_Word sh_type;   // Section type, not used
    Elf32_Word sh_flags;  // Section attributes, not used
    Elf32_Addr sh_addr;   // Virtual address in memory
    Elf32_Off  sh_offset; // Offset from start of file in bytes 
    Elf32_Word sh_size;
    Elf32_Word sh_link;
    Elf32_Word sh_info;
    Elf32_Word sh_addralign;
    Elf32_Word sh_entsize;
} Elf32_Shdr;


#endif