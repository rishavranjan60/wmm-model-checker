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
    app.add_flag("--memory-size", memory_size)->default_val(memory_size);

    size_t thread_count = 2;
    app.add_flag("--thread-count", thread_count)->default_val(thread_count);

    bool show_thread_silent = false;
    app.add_flag("--show-thread-silent,-s", show_thread_silent)->default_val(show_thread_silent);

    bool is_interactive_mode = false;
    app.add_flag("--interactive,-i", is_interactive_mode)->default_val(is_interactive_mode);

    bool is_verbose = false;
    app.add_flag("--verbose,-v", is_verbose, "Print more info about execution")->default_val(is_verbose);

    CLI11_PARSE(app, argc, argv);

    std::shared_ptr<PathChooser> path_chooser;
    if (is_interactive_mode) {
        path_chooser = std::make_shared<InteractiveChooser>();
    } else if (is_verbose) {
        path_chooser = std::make_shared<InteractiveRandomChooser>();
    } else {
        path_chooser = std::make_shared<RandomChooser>();
    }

    MemoryModel model = mem_model_from_string.at(mem_model_str);
    std::ifstream source_file{filename};
    Tokenizer tokenizer{source_file};
    auto code = Parse(tokenizer);

    if (!is_model_check) {
        path_chooser->PrintHelp();
        Program program(std::move(code), thread_count, std::move(path_chooser));
        program.Init(model, memory_size);
        program.SetVerbosity(is_verbose);
        program.SetSkipThreadSilent(!show_thread_silent);
        program.Run();
    } else {
        if (is_interactive_mode) {
            std::cerr << "WARNING: interactive mode ignored in model checking\n";
        }
        auto checker = std::make_shared<FullChooser>();
        Program program(std::move(code), thread_count, checker);

        try {
            while (!checker->Finished()) {
                program.Init(model, memory_size);
                program.SetVerbosity(is_verbose);
                program.Run();
                checker->NextRun();
            }
            std::cout << "Program is correct\n";
        } catch (FailError&) {
            auto trace = checker->GetTrace();
            std::cout << "\"fail\" found, stacktrace:\n";
            for (auto i : trace) {
                std::cout << i << '\n';
            }
            return 1;
        }
    }

    return 0;
}
