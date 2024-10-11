#include <getopt.h>

#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "../../inc/Linker/Linker.hpp"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Error: no arguments provided." << std::endl;
        return -1;
    }

    // clang-format off
    struct option long_options[] = {
        {"place", required_argument, nullptr, 0},
        {"hex", no_argument, nullptr, 0},
        {0, 0, 0, 0}
    };
    // clang-format on

    int opt_val;
    int option_index;
    int is_hex = 0;
    const char* output_file_name = nullptr;

    while ((opt_val = getopt_long_only(argc, argv, "o:", long_options, &option_index)) != -1) {
        switch (opt_val) {
            case 'o':
                if (optarg == nullptr) {
                    std::cerr << "Error: incorrect -o argument usage in " << argv[0] << "." << std::endl;
                    std::cerr << "Correct usage: " << argv[0] << " -o <output_file>" << std::endl;
                    return -1;
                }
                output_file_name = optarg;
                break;
            case 0:
                switch (option_index) {
                    case 0:
                        if (optarg == nullptr) {
                            std::cerr << "Error: incorrect --place argument usage." << std::endl;
                            std::cerr << "Correct usage: " << argv[0] << " --place=<section>@<address>" << std::endl;
                            return -1;
                        }
                        else {
                            // split optarg into section and address
                            std::string argument = std::string(optarg);
                            size_t at_pos = argument.find('@');
                            if (at_pos == std::string::npos) {
                                std::cerr << "Error: incorrect --place argument usage." << std::endl;
                                std::cerr << "Correct usage: " << argv[0] << " --place=<section>@<address>"
                                          << std::endl;
                                return -1;
                            }
                            std::string section = argument.substr(0, at_pos);
                            std::string address = argument.substr(at_pos + 1);
                            Linker::addArgument({section, (Elf32_Addr) std::stoul(address, nullptr, 0)});
                        }
                        break;
                    case 1:
                        if (is_hex != 0) {
                            std::cerr << "Error: -hex argument already provided." << std::endl;
                            return -1;
                        }
                        is_hex = 1;
                        break;
                    default:
                        break;
                }
                break;
            default:
                std::cerr << "Error: invalid arguments." << std::endl;
                return -1;
                break;
        }
    }

    std::vector<std::string> object_file_names;
    if (optind >= argc) {
        std::cerr << "Error: object files are missing." << std::endl;
        return -1;
    }
    else {
        for (int i = optind; i < argc; i++) object_file_names.push_back(argv[i]);
    }
    if (is_hex == 0) {
        std::cerr << "Error: -hex argument not provided." << std::endl;
        return -1;
    }
    if (output_file_name == nullptr) {
        std::cerr << "Error: -o argument not provided." << std::endl;
        return -1;
    }
    if (Linker::startLinking(output_file_name, object_file_names) != 0) {
        std::cerr << "Error: linking failed." << std::endl;
        return -1;
    }

    return 0;
}