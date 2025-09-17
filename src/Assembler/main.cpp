#include "Assembler/Assembler.hpp"

#include <getopt.h>
#include <iostream>

extern int yylex();
extern int yyparse();
extern FILE* yyin;

extern uint32_t line;

Assembler* assembler = nullptr;

inline void bad_argument_usage(const std::string& program_name)
{
    std::cerr << "Assembler Error: incorrect argument usage.\n"
              << "Correct usage: " << program_name << " -o <output_file> <input_file>" << std::endl;
}

int main(int argc, char* argv[])
{
    if (argc != 4) {
        bad_argument_usage(argv[0]);
        return -1;
    }

    std::string input_file_name;
    std::string output_file_name;

    int opt_val;
    while ((opt_val = getopt(argc, argv, "o:")) != -1) {
        switch (opt_val) {
        case 'o':
            if (optarg == nullptr || optind != argc - 1) {
                bad_argument_usage(argv[0]);
                return -1;
            }
            output_file_name = optarg;
            input_file_name = argv[optind];
            break;
        default:
            bad_argument_usage(argv[0]);
            return -1;
            break;
        }
    }

    try {
        assembler = new Assembler();
        assembler->start_assembler(input_file_name, output_file_name);
    }
    catch (const std::exception& e) {
        std::cerr << "Assembler error: " << e.what() << std::endl;
        delete assembler;

        return -1;
    }

    delete assembler;

    return 0;
}
