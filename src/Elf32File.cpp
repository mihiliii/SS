#include "../inc/Elf32File.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "../inc/CustomSection.hpp"
#include "../inc/RelocationTable.hpp"
#include "../inc/StringTable.hpp"
#include "../inc/SymbolTable.hpp"

Elf32File::Elf32File(std::string _file_name, bool _write)
    : elf32_header(), sh_table(), str_table(nullptr), sym_table(nullptr), custom_sections(), relocation_tables() {
    std::fstream file;
    if (_write == ELF32FILE_EMPTY) {
        file.open(_file_name, std::ios::out | std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file in Elf32File::Elf32File: " << _file_name << std::endl;
        }

        str_table = new StringTable(this);
        sym_table = new SymbolTable(this);

        elf32_header.e_shoff = sizeof(Elf32_Ehdr);
        elf32_header.e_shentsize = sizeof(Elf32_Shdr);

        str_table->add("");
        str_table->header().sh_name = str_table->add(".strtab");
        str_table->header().sh_type = SHT_STRTAB;
        str_table->header().sh_addralign = 1;

        sym_table->header().sh_name = str_table->add(".symtab");
        sym_table->header().sh_type = SHT_SYMTAB;
        sym_table->header().sh_entsize = sizeof(Elf32_Sym);
        sym_table->header().sh_addralign = 4;

        return;
    }
    else {
        file.open(_file_name, std::ios::in | std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file in Elf32File::Elf32File" << _file_name << std::endl;
        }

        file.read((char*) (&elf32_header), sizeof(Elf32_Ehdr));

        for (int sht_entry = 0; sht_entry < elf32_header.e_shnum; sht_entry++) {
            Elf32_Shdr section_header;
            file.seekg(elf32_header.e_shoff + sht_entry * sizeof(Elf32_Shdr));
            file.read((char*) (&section_header), sizeof(Elf32_Shdr));

            if (section_header.sh_type == SHT_STRTAB) {
                std::vector<char> string_table_data(section_header.sh_size);
                file.seekg(section_header.sh_offset);
                file.read((char*) string_table_data.data(), section_header.sh_size);

                str_table = new StringTable(this, section_header, string_table_data);
            }
            else if (section_header.sh_type == SHT_SYMTAB) {
                std::vector<Elf32_Sym> symbol_table_data(section_header.sh_size / sizeof(Elf32_Sym));
                file.seekg(section_header.sh_offset);
                file.read((char*) symbol_table_data.data(), section_header.sh_size);

                sym_table = new SymbolTable(this, section_header, symbol_table_data);
            }
            else if (section_header.sh_type == SHT_CUSTOM) {
                std::vector<char> custom_section_data(section_header.sh_size);
                file.seekg(section_header.sh_offset);
                file.read((char*) custom_section_data.data(), section_header.sh_size);

                std::string section_name;
                file.seekg(elf32_header.e_stroff);
                file.read((char*) section_name.data(), section_header.sh_name);
                new CustomSection(this, section_name, section_header, custom_section_data);
            }
            else if (section_header.sh_type == SHT_RELA) {
                std::vector<Elf32_Rela> relocation_table_data(section_header.sh_size / sizeof(Elf32_Rela));
                file.seekg(section_header.sh_offset);
                file.read((char*) relocation_table_data.data(), section_header.sh_size);

                std::string section_name;
                file.seekg(elf32_header.e_stroff);
                file.read((char*) section_name.data(), section_header.sh_name);
                CustomSection* parent_section = custom_sections[section_name];
                new RelocationTable(this, parent_section, section_header, relocation_table_data);
            }
        }
    }
}

void Elf32File::write(std::string _file_name, bool _mode) {
    std::ofstream file;
    if (_mode == ELF32FILE_WRITE_BIN) {
        file.open(_file_name, std::ios::out | std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file in Elf32File::writeToFile" << _file_name << std::endl;
            return;
        }

        // Write sections right after the ELF header:
        file.seekp(sizeof(Elf32_Ehdr), std::ios::beg);

        for (auto iterator : custom_sections) {
            iterator.second->write(&file);
        }

        elf32_header.e_stroff = file.tellp();
        // Write the string table:
        str_table->write(&file);

        // Write the symbol table:
        sym_table->write(&file);

        // Set section header table offset and number of entries in the ELF header:
        elf32_header.e_type = ET_REL;
        elf32_header.e_shoff = file.tellp();
        elf32_header.e_shnum = sh_table.size();

        // Write the section header table:
        for (Elf32_Shdr* section_header : sh_table) {
            file.write((char*) section_header, sizeof(Elf32_Shdr));
        }

        // Write the ELF header at the beginning of the file:
        file.seekp(0, std::ios::beg);
        file.write((char*) &elf32_header, sizeof(Elf32_Ehdr));
    }
    else if (_mode == ELF32FILE_WRITE_TXT) {
        file.open(_file_name, std::ios::out);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file in Elf32File::writeToFile: " << _file_name << std::endl;
            return;
        }

        int len = (90 - (std::string(_file_name).length() + 9)) / 2;

        file << std::setw(len) << std::setfill('*') << "   " << "ELFREAD: " << _file_name << std::setw(len)
             << std::setfill('*') << std::left << "   " << "\n\n";

        // Write ELF header
        file << "Elf Header:" << std::endl;

        switch (elf32_header.e_type) {
            case ET_NONE:
                file << "  Type: No file type" << std::endl;
                break;
            case ET_REL:
                file << "  Type: Relocatable file" << std::endl;
                break;
            case ET_EXEC:
                file << "  Type: Executable file" << std::endl;
                break;
            case ET_DYN:
                file << "  Type: Shared object file" << std::endl;
                break;
            default:
                break;
        }

        file << std::hex;
        file << "  Entry point address: 0x" << elf32_header.e_entry << std::endl;
        file << "  Section header offset: 0x" << elf32_header.e_shoff << std::endl;
        file << std::dec;
        file << "  Section header entry size: " << elf32_header.e_shentsize << " (bytes)" << std::endl;
        file << "  Number of section headers: " << elf32_header.e_shnum << std::endl;
        file << std::hex;
        file << "  Page header offset: 0x" << elf32_header.e_phoff << std::endl;
        file << std::dec;
        file << "  Page header entry size: " << elf32_header.e_phentsize << " (bytes)" << std::endl;
        file << "  Number of page headers: " << elf32_header.e_phnum << std::endl;
        file << std::hex;
        file << "  String table offset: 0x" << elf32_header.e_stroff << std::endl;

        file << std::dec;
        // Write section header table
        file << std::endl << "Section Header Table:" << std::endl;
        file << "  ";
        file << std::left << std::setfill(' ');
        file << std::setw(4) << "NUM";
        file << std::setw(25) << "NAME";
        file << std::setw(5) << "TYPE";
        file << std::setw(9) << "ADDRESS";
        file << std::setw(9) << "OFFSET";
        file << std::setw(9) << "SIZE";
        file << std::setw(5) << "LINK";
        file << std::setw(5) << "INFO";
        file << std::setw(6) << "ALIGN";
        file << std::setw(9) << "ENTSIZE";
        file << std::endl;
        uint32_t index = 0;
        for (auto& iterator : sh_table) {
            Elf32_Shdr* section = iterator;
            file << "  ";
            file << std::setw(3) << std::right << std::dec << std::setfill(' ') << index << " ";
            file << std::left;
            file << std::setw(24) << str_table->get(section->sh_name) << " ";
            file << std::right << std::hex << std::setfill('0');
            file << std::setw(4) << section->sh_type << " ";
            file << std::setw(8) << section->sh_addr << " ";
            file << std::setw(8) << section->sh_offset << " ";
            file << std::setw(8) << section->sh_size << " ";
            file << std::setw(4) << section->sh_link << " ";
            file << std::setw(4) << section->sh_info << " ";
            file << std::setw(5) << std::dec << std::setfill(' ') << std::left << section->sh_addralign << " ";
            file << std::setw(8) << section->sh_entsize << std::endl;
            index++;
        }

        for (auto iterator : custom_sections) {
            iterator.second->print(file);
            RelocationTable* relocation_table = iterator.second->getRelocationTable();
            if (!relocation_table->isEmpty()) {
                relocation_table->print(file);
            }
        }

        sym_table->print(file);
    }
    file.close();
}

void Elf32File::writeRawContent(std::string _input_file, std::string _output_file) {
    size_t bufferSize = 1024;
    std::vector<char> buffer(bufferSize);

    std::ifstream f_input(_input_file, std::ios::binary | std::ios::in);
    if (!f_input.is_open()) {
        std::cerr << "Error: Could not open file in Elf32File::writeRawContent: " << _input_file << std::endl;
        return;
    }

    std::ofstream f_output(_output_file, std::ios::out);
    if (!f_output.is_open()) {
        std::cerr << "Error: Could not open f_output in Elf32File::writeRawContent: " << _output_file << std::endl;
        return;
    }

    f_output << std::endl << "Content of f_output: " << _input_file << ":\n";
    while (!f_input.eof()) {
        f_input.read(buffer.data(), bufferSize);
        size_t s = f_input.gcount();
        for (size_t i = 0; i < s; i++) {
            if (i % 16 == 0)
                f_output << std::right << std::hex << std::setw(8) << std::setfill('0') << i << ": ";
            else if (i % 8 == 0)
                f_output << std::dec << " ";

            f_output << std::hex << std::setw(2) << std::setfill('0') << (unsigned int) (unsigned char) buffer[i]
                     << " ";

            if ((i + 1) % 16 == 0) {
                f_output << std::dec << " |";
                for (size_t j = i - 15; j < i + 1; j++) {
                    if (buffer[j] < 32 || buffer[j] > 126) {
                        f_output << ".";
                    }
                    else {
                        f_output << buffer[j];
                    }
                    if ((j + 1) % 16 == 0) {
                        f_output << std::dec << "|\n";
                    }
                }
            }
        }

        if (s % 16 != 0) {
            for (size_t i = 0; i < 16 - s % 16; i++) {
                f_output << "   ";
                if ((s + i) % 8 == 0) {
                    f_output << " ";
                }
            }
            f_output << std::dec << " |";
            for (size_t i = s - s % 16; i < s; i++) {
                if (buffer[i] < 32 || buffer[i] > 126) {
                    f_output << ".";
                }
                else {
                    f_output << buffer[i];
                }
                if (i == s - 1) {
                    f_output << std::dec << "|\n";
                }
            }
        }
    }

    f_output << std::dec << std::endl;

    f_input.close();
    f_output.close();
}
