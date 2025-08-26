#include <getopt.h>

#include "../../inc/Linker/Linker.hpp"

#include <cstring>
#include <iostream>
#include <string>
#include <vector>

Linker* linker = nullptr;

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "Error: no arguments provided." << std::endl;
        return -1;
    }

    struct option long_options[] = {
        {"place", required_argument, nullptr, 0},
        {  "hex",       no_argument, nullptr, 0},
        {      0,                 0,       0, 0}
    };

    int opt_val;
    int option_index;

    bool is_hex = false;
    const char* output_file_name;

    linker = new Linker();

    while ((opt_val = getopt_long_only(argc, argv, "o:", long_options, &option_index)) != -1) {
        enum OPT_TYPE { LONG = 0, SHORT = 'o' };
        enum OPT_LONG { PLACE = 0, HEX = 1 };

        switch (opt_val) {
        case OPT_TYPE::SHORT:
            if (optarg == nullptr) {
                std::cerr << "Error: incorrect -o argument usage, output file name is missing.\n"
                          << "Correct usage: -o <output_file>" << std::endl;
                return -1;
            }

            output_file_name = optarg;
            break;
        case OPT_TYPE::LONG:
            switch (option_index) {
            case PLACE:
                if (optarg == nullptr) {
                    std::cerr << "Error: incorrect --place argument usage.\n"
                              << "Correct usage: --place=<section>@<address>" << std::endl;
                    return -1;
                }
                else {
                    const std::string argument = optarg;
                    size_t delimiter_loc = argument.find('@');

                    if (delimiter_loc == std::string::npos) {
                        std::cerr << "Error: incorrect --place argument usage.\n"
                                  << "Correct usage: --place=<section>@<address>" << std::endl;
                        return -1;
                    }

                    const std::string section_name = argument.substr(0, delimiter_loc);
                    const std::string address = argument.substr(delimiter_loc + 1);

                    linker->add_argument(
                        {section_name, (Elf32_Addr) std::stoul(address, nullptr, 0)});
                }
                break;
            case HEX:
                if (is_hex) {
                    std::cerr << "Error: multiple -hex arguments provided." << std::endl;
                    return -1;
                }
                is_hex = true;
                break;
            default:
                std::cerr << "Error: unrecognized command line option." << argv[optind - 1]
                          << std::endl;
                return -1;
            }
            break;
        default:
            std::cerr << "Error: unrecognized command line option." << argv[optind - 1]
                      << std::endl;
            return -1;
        }
    }

    if (optind >= argc) {
        std::cerr << "Error: input object files are missing." << std::endl;
    }
    else if (!is_hex) {
        std::cerr << "Error: -hex argument not provided." << std::endl;
    }
    else if (output_file_name == nullptr) {
        std::cerr << "Error: -o argument not provided." << std::endl;
    }
    else {
        std::vector<std::string> input_file_names;
        for (int i = optind; i < argc; i++) {
            input_file_names.push_back(argv[i]);
        }
        linker->start_linker(output_file_name, input_file_names);
    }

    // FIX: delete linker object

    return -1;
}
