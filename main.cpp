#include "defs.h"
#include "program.h"
#include "tokenizer.h"
#include "parser.h"
#include "path_choosers/interactive.h"
#include "path_choosers/full.h"
#include "path_choosers/random.h"

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
    constexpr bool is_model_check = true;
    constexpr size_t kMemSize = 8;
    constexpr auto model = MemoryModel::SRA;
    if (!is_model_check) {
        Program program(std::move(code), 2, std::make_shared<InteractiveChooser>());
        program.Init(model, kMemSize);
        program.Run();
    } else {
        auto checker = std::make_shared<FullChooser>();
        Program program(std::move(code), 2, checker);

        try {
            while (!checker->Finished()) {
                program.Init(model, kMemSize);
                program.Run();
                checker->NextRun();
            }
        } catch (FailError&) {
            auto trace = checker->GetTrace();
            std::cout << "\"fail\" founded, stacktrace:\n";
            for (auto i : trace) {
                std::cout << i << '\n';
            }
        }
    }

    return 0;
}
