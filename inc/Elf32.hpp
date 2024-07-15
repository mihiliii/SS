#ifndef Elf32_hpp_
#define Elf32_hpp_

#include "Types.hpp"

typedef uint32_t Elf32_Addr;
typedef uint32_t Elf32_Off;
typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Word;
typedef int32_t  Elf32_SWord;


#define EI_NIDENT       16
#define EI_MAG0         0 /* File identification byte 0 index */
#define EI_MAG1         1 /* File identification byte 1 index */
#define EI_MAG2         2 /* File identification byte 2 index */
#define EI_MAG3         3 /* File identification byte 3 index */
#define EI_CLASS        4 /* File class byte index */
#define EI_DATA         5 /* Data encoding byte index */
#define EI_VERSION      6 /* File version byte index */
#define EI_OSABI        7 /* OS ABI identification */
#define EI_ABIVERSION   8 /* ABI version */
#define EI_PAD          9 /* Byte index of padding bytes */

// Struktura koja predstavlja header same ELF datoteke:
struct Elf32_Ehrd { 
    unsigned char e_ident[EI_NIDENT] {0x7f, 'E', 'L', 'F'};
    Elf32_Half e_type;
    Elf32_Half e_machine;
    Elf32_Word e_version;
    Elf32_Addr e_entry;
    Elf32_Off e_phoff;
    Elf32_Off e_shoff;
    Elf32_Word e_flags;
    Elf32_Half e_ehsize;
    Elf32_Half e_phentsize;
    Elf32_Half e_phnum;
    Elf32_Half e_shentsize;
    Elf32_Half e_shnum;
    Elf32_Half e_shstrndx;
};


// Struktura koja predstavlja sekciju tabele simbola
struct Elf32_Shrd {
    uint32_t sh_offset;   // Pomeraj od pocetka ELF datoteke do lokacije gde pocinje sym table
    uint32_t sh_size;     // Velicina sekcije symbol table izrazeno u broju bajtova
    uint32_t sh_link;     // Indeks zaglavlja sekcije pridruzene tabele stringova
    uint32_t sh_info;     // Vrednost za jedan veca od indeksa poslednjeg lokalnog simbola u sym table
    uint32_t sh_entsize;  // Velicina jednog ulaza unutar sym table
};


// Struktura za ulaz u tabelu simbola:
struct Elf32_Sym {
    Elf64_Word    st_name;
    unsigned char st_info;
    Elf64_Half    st_other;
    Elf64_Addr    st_value;
    Elf64_XWord   st_size;
};


#endif