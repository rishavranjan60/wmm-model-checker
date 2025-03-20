#include "defs.h"
#include "tokenizer.h"
#include "parser.h"
#include "program.h"
#include "path_choosers/full.h"

#include <catch2/catch_test_macros.hpp>

#include <fstream>
#include <string>


namespace tso_test {
    void run_program(std::string filename) {
        MemoryModel model = MemoryModel::TSO;
        std::ifstream source_file{filename};
        Tokenizer tokenizer{source_file};
        auto code = Parse(tokenizer);

        auto checker = std::make_shared<FullChooser>();
        size_t thread_count = 2;
        bool is_verbose = false;
        size_t memory_size = 8;
        Program program(std::move(code), thread_count, checker);

        while (!checker->Finished()) {
            program.Init(model, memory_size);
            program.SetVerbosity(is_verbose);
            program.Run();
            checker->NextRun();
        }
    }

    TEST_CASE("Allowed Load-Store reordering in TSO") {
        std::string filename = "tso/programs/load_store.txt";
		run_program(filename);
    }

    TEST_CASE("FIFO behavior of the Store Buffering Passes (Store Buffering Passes)") {
        std::string filename = "tso/programs/fifo.txt";
		run_program(filename);
    }

    TEST_CASE("Recording order with RELEASE and ACQUIRE") {
		std::string filename = "tso/programs/rel_acq.txt";
        run_program(filename);
    }

    TEST_CASE("Global Store Ordering") {
		std::string filename = "tso/programs/global_store_ordering.txt";
        run_program(filename);
    }
}