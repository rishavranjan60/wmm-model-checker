#include "defs.h"
#include "tokenizer.h"
#include "parser.h"
#include "program.h"
#include "path_choosers/full.h"

#include <catch2/catch_test_macros.hpp>
#include <fstream>
#include <string>

namespace pso_test {

const std::string kStoreReordering = R"(
r0 = + r0 r15
r1 = 1
if r0 goto t2+
if r1 goto t1
t2+: r0 = - r0 r1
     if r0 goto bad
     if r1 goto t2
t1: r0 = 0
    r1 = 1
    r2 = 1
    store RLX #r0 r2
    store RLX #r1 r2
    finish
t2: load RLX #r1 r11
    load RLX #r0 r10
    if r11 goto check
    finish
check: finish
bad: fail
end: finish

-----
In PSO, stores to different addresses can be reordered.

This test verifies if the memory model allows the second thread to see y = 1 (r11 = 1)
without seeing x = 1 (r10 = 0), which is permitted under PSO but would fail under TSO.
)";

void RunProgram(const std::string& code_text) {
    MemoryModel model = MemoryModel::PSO;
    std::stringstream ss(code_text);
    Tokenizer tokenizer{ss};
    auto code = Parse(tokenizer);

    auto checker = std::make_shared<FullChooser>();
    bool is_verbose = false;
    size_t memory_size = 8;
    size_t thread_count = 2;
    Program program(std::move(code), thread_count, checker);

    while (!checker->Finished()) {
        program.Init(model, memory_size);
        program.SetVerbosity(is_verbose);
        program.Run();
        checker->NextRun();
    }
}

TEST_CASE("Store Reordering in PSO") {
    std::string code_text = kStoreReordering;
    RunProgram(code_text);
}

}  // namespace pso_test
