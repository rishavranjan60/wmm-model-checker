#include "defs.h"
#include "program.h"
#include "tokenizer.h"
#include "parser.h"
#include "path_chooser.h"

#include <iostream>
#include <fstream>
#include <memory>

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << std::string{"Usage: "} + argv[0] + " <source-code-file>\n";
    }

    std::ifstream source_file{argv[1]};
    if (!source_file.is_open()) {
        std::cerr << std::string{"Can't open \""} + argv[1] + "\" file\n";
    }

    Tokenizer tokenizer{source_file};
    auto code = Parse(tokenizer);
    Program program(std::move(code), 1, std::make_unique<RandomChooser>(239));
    program.Init(MemoryModel::TSO);

    program.Run();

    return 0;
}
