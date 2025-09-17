#include "Elf32/Elf32File.hpp"

#include <fstream>
#include <iomanip>

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <elf32_file_name>" << std::endl;
        return -1;
    }

    std::fstream input_file(argv[1], std::ios::binary | std::ios::in);
    if (!input_file.is_open()) {
        std::cerr << "ReadElf32 Error: Could not open file: " << argv[1] << std::endl;
        return -1;
    }

    Elf32File elf_file = Elf32File(argv[1]);

    std::cout << "--- Reading ELF32 file: " << argv[1] << " ---" << std::endl;

    elf_file.read_elf();

    const size_t BUFFER_SIZE = 1024;
    std::vector<Elf32_Byte> buffer(BUFFER_SIZE);

    std::cout << "Content of Elf32File " << argv[1] << ":" << std::endl;
    std::cout << std::right << std::hex << std::setfill('0');

    const char MIN_ASCII_CHAR = 32;
    const char MAX_ASCII_CHAR = 126;
    const size_t LINE_WIDTH = 16;

    size_t write_count = 0;

    while (!input_file.eof()) {
        input_file.read((char*) buffer.data(), BUFFER_SIZE);
        size_t read_count = input_file.gcount();

        for (size_t i = 0; i < read_count; i++, write_count++) {
            if (write_count % LINE_WIDTH == 0) {
                std::cout << std::setw(8) << write_count << ": ";
            }
            else if (write_count % (LINE_WIDTH / 2) == 0) {
                std::cout << " ";
            }

            std::cout << std::setw(2) << (uint32_t) buffer[i] << " ";

            if ((write_count + 1) % LINE_WIDTH == 0) {
                std::cout << " |";
                for (size_t j = i; j < i + LINE_WIDTH; j++) {
                    const size_t INDEX = j - (LINE_WIDTH - 1);

                    if (buffer[INDEX] < MIN_ASCII_CHAR || buffer[INDEX] > MAX_ASCII_CHAR) {
                        std::cout << ".";
                    }
                    else {
                        std::cout << buffer[INDEX];
                    }
                }
                std::cout << "|\n";
            }
        }

        if (read_count % LINE_WIDTH != 0) {
            for (size_t j = 0; j < LINE_WIDTH - (read_count % LINE_WIDTH); j++) {
                std::cout << "   ";

                if ((read_count + j) % (LINE_WIDTH / 2) == 0) {
                    std::cout << " ";
                }
            }
            std::cout << " |";
            for (size_t j = read_count - read_count % LINE_WIDTH; j < read_count; j++) {

                if (buffer[j] < MIN_ASCII_CHAR || buffer[j] > MAX_ASCII_CHAR) {
                    std::cout << ".";
                }
                else {
                    std::cout << buffer[j];
                }
            }
            std::cout << "|\n";
        }
    }

    std::cout << std::endl;
    input_file.close();

    return 0;
}
