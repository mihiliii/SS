#include "../../inc/Assembler/Assembler.hpp"
#include <getopt.h>
#include <iostream>

extern int yylex();
extern int yyparse();
extern FILE* yyin;

extern uint32_t line;

inline int bad_argument_usage_error(const std::string& program_name)
{
    std::cerr << "Error, incorrect argument usage in " << program_name << ".\n"
              << "Correct usage: " << program_name << " -o <output_file> <input_file>" << std::endl;
    return -1;
}

Assembler* assembler = nullptr;

int main(int argc, char* argv[])
{
    if (argc != 4) {
        bad_argument_usage_error(argv[0]);
    }

    std::string input_file_name;
    std::string output_file_name;

    int opt_val;
    while ((opt_val = getopt(argc, argv, "o:")) != -1) {
        switch (opt_val) {
        case 'o':
            if (optarg == nullptr || optind != argc - 1) {
                bad_argument_usage_error(argv[0]);
            }
            output_file_name = optarg;
            input_file_name = argv[optind];
            break;
        default:
            bad_argument_usage_error(argv[0]);
            break;
        }
    }

    // FIX: delete assembler object
    assembler = new Assembler();
    assembler->start_assembler(input_file_name, output_file_name);

    return 0;
}
