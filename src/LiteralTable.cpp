#include "../inc/LiteralTable.hpp"

#include <fstream>

#include "../inc/CustomSection.hpp"

bool LiteralTable::isEmpty() { return literal_table.empty(); }

void LiteralTable::addLiteralReference(int _literal, Elf32_Addr _address) {
    if (literal_table.find(_literal) == literal_table.end()) {
        literal_table[_literal] = std::make_pair(literal_pool.size() * sizeof(int), std::list<Elf32_Addr>());
        literal_pool.emplace_back(_literal);
    }
    literal_table[_literal].second.push_back(_address);
}

Elf32_Off LiteralTable::addLiteralToPool(int _literal) {
    literal_pool.emplace_back(_literal);
    return (literal_pool.size() * sizeof(uint32_t) - 4);
}

Elf32_Off LiteralTable::getLiteralOffset(int _literal) {
    if (literal_table.find(_literal) != literal_table.end()) {
        return literal_table[_literal].first;
    }
    return -1;
}

void LiteralTable::writePool(std::ofstream* _file) {
    _file->write((char*) literal_pool.data(), literal_pool.size() * sizeof(int));
}

void LiteralTable::resolveLiteralReferences() {
    for (const auto& entry : literal_table) {
        for (const Elf32_Off& section_offset : entry.second.second) {
            Elf32_Off literal_pool_offset = entry.second.first;
            uint8_t* content = (uint8_t*) parent_section->getContent(section_offset);

            uint32_t disp = literal_pool_offset + (parent_section->getLocationCounter() - section_offset) - 4;

            if (disp > 0xFFF) {
                std::cout << "Literal pool overflow" << std::endl;
            }

            uint32_t new_content = content[3] << 24 | content[2] << 16 | (content[1] & 0xF0) << 8 | (disp & 0xFFF);

            std::cout << new_content;
            parent_section->overwriteContent(&new_content, sizeof(uint32_t), section_offset);
        }
    }
}
