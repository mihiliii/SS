#include <getopt.h>

#include "Linker/Linker.hpp"

#include <cstring>
#include <iostream>
#include <string>

inline void bad_argument_usage(const std::string& program_name)
{
    std::cerr << "Linker Error: incorrect argument usage.\n"
              << "Correct usage: " << program_name
              << " -o <output_file> --place=<section>@<address> -hex <input_files...>" << std::endl;
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        bad_argument_usage(argv[0]);
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

    std::map<std::string, Elf32_Addr> place_addresses;

    while ((opt_val = getopt_long_only(argc, argv, "o:", long_options, &option_index)) != -1) {
        enum OPT_TYPE { LONG = 0, SHORT = 'o' };
        enum OPT_LONG { PLACE = 0, HEX = 1 };

        switch (opt_val) {
        case OPT_TYPE::SHORT:
            if (optarg == nullptr) {
                bad_argument_usage(argv[0]);
                return -1;
            }

            output_file_name = optarg;
            break;
        case OPT_TYPE::LONG:
            switch (option_index) {
            case PLACE:
                if (optarg == nullptr) {
                    bad_argument_usage(argv[0]);
                    return -1;
                }
                else {
                    const std::string argument = optarg;
                    size_t delimiter_pos = argument.find('@');

                    if (delimiter_pos == std::string::npos) {
                        bad_argument_usage(argv[0]);
                        return -1;
                    }

                    const std::string section_name = argument.substr(0, delimiter_pos);
                    Elf32_Addr address = std::stoul(argument.substr(delimiter_pos + 1), nullptr, 0);

                    place_addresses.emplace(section_name, address);
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

    std::list<Elf32File*> input_files;
    for (int i = optind; i < argc; i++) {
        input_files.emplace_back(new Elf32File(argv[i]));
    }

    try {
        Linker linker = Linker(input_files, place_addresses);
        linker.start_linker(output_file_name);
    }
    catch (const std::exception& e) {
        std::cerr << "Linker error: " << e.what() << std::endl;

        for (auto& file : input_files) {
            delete file;
        }

        return -1;
    }

    for (auto& file : input_files) {
        delete file;
    }

    return 0;
}
