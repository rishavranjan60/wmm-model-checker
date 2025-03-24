#include "defs.h"
#include "parser.h"
#include "path_choosers/full.h"
#include "program.h"
#include "tokenizer.h"

#include <catch2/catch_test_macros.hpp>
#include <fstream>
#include <string>

namespace pso_test
{

const std::string kStoreReordering = R"(
r0 = + r0 r15
r1 = 1
if r0 goto t2+
if r1 goto t1
t2+: r0 = - r0 r1
     if r0 goto bad
     if r1 goto t2
t1:  r0 = 0
     r1 = 1
     r2 = 1
     store RLX #r0 r2
     store RLX #r1 r2
     finish
t2:  load RLX #r1 r11
     load RLX #r0 r10
     if r11 goto check
     finish
check: finish
bad: fail
end: finish

-----
In PSO, store-store reordering is allowed between different addresses.

1) Thread 1 writes x=1 then y=1. 
2) Thread 2 may see y=1 before x=1, which is valid in PSO (fail NOT expected).
3) If r11 == 1 and r10 == 0 — PSO allows this (no fail triggered). This is allowed behavior.
)";

const std::string kStoreReorderingViolationWithFence = R"(
r0 = + r0 r15
r1 = 1
if r0 goto t2+
if r1 goto t1
t2+: r0 = - r0 r1
     if r0 goto bad
     if r1 goto t2
t1:  r0 = 0
     r1 = 1
     r2 = 1
     store RLX #r0 r2
     fence SEQ_CST
     store RLX #r1 r2
     finish
t2:  load RLX #r1 r11
     load RLX #r0 r10
     if r11 goto check
     finish
check: finish
bad: fail
end: finish

-----
In PSO, fence SEQ_CST should prevent reordering.

1) Thread 1 writes x=1, does SEQ_CST fence, then y=1.
2) Thread 2 loads y then x.
3) If it sees y=1 but x=0, that violates ordering → fail is triggered.
)";

void RunProgram(const std::string &code_text)
{
    MemoryModel model = MemoryModel::PSO;
    std::stringstream ss(code_text);
    Tokenizer tokenizer{ss};
    auto code = Parse(tokenizer);

    auto checker = std::make_shared<FullChooser>();
    bool is_verbose = false;
    size_t memory_size = 8;
    size_t thread_count = 2;
    Program program(std::move(code), thread_count, checker);

    while (!checker->Finished())
    {
        program.Init(model, memory_size);
        program.SetVerbosity(is_verbose);
        program.Run();
        checker->NextRun();
    }
}

TEST_CASE("Allowed store-store reordering in PSO")
{
    std::string code_text = kStoreReordering;
    RunProgram(code_text);
}

TEST_CASE("Store-store ordering enforced with SEQ_CST fence")
{
    std::string code_text = kStoreReorderingViolationWithFence;
    RunProgram(code_text);
}

} // namespace pso_test
