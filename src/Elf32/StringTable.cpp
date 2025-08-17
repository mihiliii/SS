#include "../../inc/Elf32/StringTable.hpp"

#include "../../inc/Elf32/Elf32File.hpp"

StringTable::StringTable(Elf32File& elf32_file)
    : Section(elf32_file,
              {
                  .sh_name = 0,
                  .sh_type = SHT_STRTAB,
                  .sh_addr = 0,
                  .sh_offset = 0,
                  .sh_size = 0,
                  .sh_link = 0,
                  .sh_info = 0,
                  .sh_addralign = 1,
                  .sh_entsize = 1,
              }),
      _string_table()
{
    add_string("\0");
    // _string_table.insert(std::make_pair(0, ""));
    _header.sh_name = add_string(".strtab");
}

const Elf32_Off StringTable::add_string(const std::string& string)
{
    Elf32_Off offset = _header.sh_size;
    _header.sh_size += string.size() + 1;

    _string_table.insert(std::make_pair(offset, string));
    return offset;
}

const std::string& StringTable::get_string(Elf32_Off offset) const
{
    if (_string_table.find(offset) == _string_table.end()) {
        return _string_table.at(0);
    }
    else {
        return _string_table.at(offset);
    }
}

Elf32_Off StringTable::get_offset(const std::string& string) const
{
    for (const auto& pair : _string_table) {
        const Elf32_Off& offset = pair.first;
        const std::string& str = pair.second;
        if (str == string) {
            return offset;
        }
    }
    return 0;
}

void StringTable::set_string_table(const std::vector<char>& str_table_data)
{
    _string_table.clear();
    for (int i = 0; i < (int) str_table_data.size(); i++) {
        Elf32_Off offset = i;
        std::string str;
        while (str_table_data[i] != '\0') {
            str += str_table_data[i++];
        }
        _string_table.try_emplace(offset, str);
    }
}

void StringTable::write(std::ostream& ostream)
{
    _header.sh_offset = ostream.tellp();

    for (const auto& pair : _string_table) {
        const std::string& str = pair.second;
        ostream.write(str.c_str(), str.size() + 1);
    }
}

void StringTable::print(std::ostream& ostream) const
{
    ostream << "String Table: " << _header.sh_name << "\n";
    for (const auto& pair : _string_table) {
        const Elf32_Off& offset = pair.first;
        const std::string& str = pair.second;
        ostream << "Offset: " << offset << " String: " << str << "\n";
    }
}
