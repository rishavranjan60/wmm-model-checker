#include "defs.h"
#include "tokenizer.h"
#include "parser.h"
#include "program.h"
#include "path_choosers/full.h"

#include <catch2/catch_test_macros.hpp>

#include <fstream>
#include <string>

const std::string simple_sequence = R"(
r0 = + r1 r15
r1 = 1
r2 = 2
r3 = 0
if r0 goto t2
if r1 goto t1
fail
t1: store REL #r10 r1
load ACQ #r10 r3
finish
t2: store REL #r10 r2
load ACQ #r10 r4
r5 = - r3 r2
r6 = - r4 r1
if r5 goto end
if r6 goto end
fail
end: finish

-----

Sequential Consistency (SC) is a memory model where
all operations from all threads execute in a single global order,
and this order respects the program order of each individual thread.

x = 0
t1: x = 1; r3 = x
t2: x = 2; r4 = x; assert not (r3 == 2 and r4 == 1))";

const std::string mutual_dependence = R"(
r0 = + r0 r15
r1 = + r1 r15
r2 = 1
r3 = 1
r4 = 1
r10 = 1
r11 = 1
r12 = 1
if r0 goto t2
if r3 goto t3
fail
t2: if r1 goto t3
r5 = 0
store REL #r2 r5
load ACQ #r3 r11
finish
t3: r5 = 0
store REL #r3 r5
load ACQ #r4 r12
finish
t4: r5 = 0
store REL #r4 r5
load ACQ #r2 r10
r6 = * r10 r11
r6 = * r6 r12
if r6 goto fail
finish
fail: fail

-----

Sequential Consistency (SC) is a memory model where
all operations from all threads execute in a single global order,
and this order respects the program order of each individual thread.

x = y = z = 1
r10 = r11 = r12 = 0
T2: write x = 0; read y -> r11
T3: write y = 0; read z -> r12
T4: write z = 0; read x -> r10; assert r10 * r11 * r12 != 1

In the last thread to execute, the read value will be 0,
so the product of the variables will end up being 0.)";

const std::string check_variable = R"(
r0 = + r1 r15
r1 = 1
r2 = 2
r3 = 0
if r0 goto t2
if r1 goto t1
fail
t1: store REL #r10 r1
load ACQ #r10 r3
finish
t2: store REL #r10 r2
load ACQ #r10 r4
r5 = - r3 r2
r6 = - r4 r1
if r5 goto end
if r6 goto end
fail
end: finish

-----

Sequential Consistency (SC) is a memory model where
all operations from all threads execute in a single global order,
and this order respects the program order of each individual thread.

x = 0
t1: x = 1; r3 = x
t2: x = 2; r4 = x; assert not (r3 == 2 and r4 == 1))";

void run_seq_program(std::string code_text, size_t thread_count) {
    MemoryModel model = MemoryModel::SEQ_CST;
    std::stringstream ss(code_text);
    Tokenizer tokenizer{ss};
    auto code = Parse(tokenizer);

    auto checker = std::make_shared<FullChooser>();
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

TEST_CASE("Check the order of the two sequences") {
    std::string code_text = simple_sequence;
    run_seq_program(code_text, 2);
}

TEST_CASE("Check the order of the three mutual sequences") {
    std::string code_text = mutual_dependence;
    run_seq_program(code_text, 3);
}

TEST_CASE("Check the order of writing and then reading") {
    std::string code_text = check_variable;
    run_seq_program(code_text, 2);
}