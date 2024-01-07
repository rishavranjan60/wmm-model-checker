#include "defs.h"
#include "program.h"
#include "tokenizer.h"
#include "parser.h"
#include "path_choosers/interactive.h"
#include "path_choosers/full.h"
#include "path_choosers/random.h"

#include <CLI11/CLI11.hpp>

#include <iostream>
#include <fstream>
#include <memory>
#include <unordered_map>

int main(int argc, char** argv) {
    CLI::App app{"Executor under weak memory models"};

    std::string filename;
    app.add_option("input-file", filename, "File with code")->required()->check(CLI::ExistingFile);

    std::string mem_model_str = "seq_cst";
    app.add_flag("--memory-model", mem_model_str)
        ->default_val(mem_model_str)
        ->check(CLI::IsMember({"seq_cst", "tso", "pso", "ra", "sra"}));
    const std::unordered_map<std::string, MemoryModel> mem_model_from_string = {{"seq_cst", MemoryModel::SEQ_CST},
                                                                                {"tso", MemoryModel::TSO},
                                                                                {"pso", MemoryModel::PSO},
                                                                                {"ra", MemoryModel::RA},
                                                                                {"sra", MemoryModel::SRA}};

    bool is_model_check = false;
    app.add_flag("--model-checking,-c", is_model_check, "Check all possible executions");

    size_t memory_size = 8;
    app.add_flag("--memory-size,-s", memory_size)->default_val(memory_size);

    size_t thread_count = 2;
    app.add_flag("--thread-count,-t", thread_count)->default_val(thread_count);

    bool show_thread_silent = false;
    app.add_flag("--show-thread-silent", show_thread_silent)->default_val(show_thread_silent);

    bool is_interactive_mode = false;
    app.add_flag("--interactive,-i", is_interactive_mode)->default_val(is_interactive_mode);

    bool is_verbose = false;
    app.add_flag("--verbose,-v", is_verbose, "Print more info about execution")->default_val(is_verbose);

    CLI11_PARSE(app, argc, argv);

    std::shared_ptr<PathChooser> path_chooser;
    if (is_interactive_mode) {
        path_chooser = std::make_shared<InteractiveChooser>();
    } else {
        path_chooser =
            is_verbose ? std::make_shared<InteractiveRandomChooser>() : std::make_shared<InteractiveChooser>();
    }

    MemoryModel model = mem_model_from_string.at(mem_model_str);
    std::ifstream source_file{filename};
    Tokenizer tokenizer{source_file};
    auto code = Parse(tokenizer);

    if (!is_model_check) {
        Program program(std::move(code), thread_count, std::move(path_chooser));
        program.SetSkipThreadSilent(!show_thread_silent);
        program.Init(model, memory_size);
        path_chooser->PrintHint();
        program.Run();
    } else {
        auto checker = std::make_shared<FullChooser>();
        Program program(std::move(code), thread_count, checker);

        try {
            while (!checker->Finished()) {
                program.Init(model, memory_size);
                program.Run();
                checker->NextRun();
            }
            std::cout << "Program is correct\n";
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
