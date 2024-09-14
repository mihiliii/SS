#include <getopt.h>

#include <cstring>
#include <iostream>
#include <string>

#include "../../inc/Assembler/Assembler.hpp"

int main(int argc, char* argv[]) {
    const char* input_file_name = nullptr;
    const char* output_file_name = nullptr;

    int opt_val;
    while ((opt_val = getopt(argc, argv, "o:")) != -1) {
        switch (opt_val) {
            case 'o':
                if (optarg == nullptr) {
                    std::cerr << "Error, incorrect -o argument usage in " << argv[0] << "." << std::endl;
                    std::cerr << "Correct usage: " << argv[0] << " -o <output_file> <input_file>\n";
                    return -1;
                }
                output_file_name = optarg;
                if (optind != argc - 1) {
                    std::cerr << "Error, incorrect -o argument usage in " << argv[0] << "." << std::endl;
                    std::cerr << "Correct usage: " << argv[0] << " -o <output_file> <input_file>\n";
                    return -1;
                }
                input_file_name = argv[optind];
                break;
            default:
                std::cerr << "Error, invalid arguments." << std::endl;
                return -1;
                break;
        }
    }

    if (input_file_name == nullptr || output_file_name == nullptr) {
        std::cerr << "Error, invalid arguments." << std::endl;
        return -1;
    }

    if (Assembler::startAssembler(input_file_name, output_file_name) == -1) {
        std::cerr << "Error: assembler failed to start. \n";
        return -1;
    }

    return 0;
}
