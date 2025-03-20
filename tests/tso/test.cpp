#include "defs.h"
#include "tokenizer.h"
#include "parser.h"
#include "program.h"
#include "path_choosers/full.h"

#include <catch2/catch_test_macros.hpp>

#include <fstream>
#include <string>

namespace tso_test {

const std::string load_store = R"(
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
     load RLX #r0 r10
     store RLX #r1 r2
     finish
t2:  store RLX #r0 r2
     load RLX #r1 r11
     if r10 goto check
     finish
check: if r11 goto end
       fail
bad: fail
end: finish

-----
TSO allows you to rearrange the loads before writing if they belong to different addresses

This test simulates a situation where one thread first reads x and then writes y,
and another thread does the opposite (writes x, then reads y)

1) In TSO, it is allowed to transpose load(x) before store(y),
but it is forbidden to observe y = 1 without seeing x = 1

2) If r10 == 0 (the thread has seen x = 0), then r11 must be 0. If it is 1,
it means an invalid situation has occurred, fail is called)";

const std::string fifo = R"(
r0 = + r0 r15
r1 = 1
if r0 goto t2+
if r1 goto t1
t2+:  r0 = - r0 r1
      if r0 goto bad
      if r1 goto t2
t1:  r0 = 0
     r1 = 1
     r2 = 1
     store RLX #r0 r2
     load RLX #r0 r10
     finish
t2:  store RLX #r1 r2
     load RLX #r1 r11
     if r10 goto check
     finish
check: if r11 goto end
       fail
bad: fail
end: finish

-----
In TSO, each thread sees its own records before other threads do

1) In TSO, it is guaranteed that if a thread has written x = 1 itself,
it will surely see x = 1 when it loads it

2) Similarly, another thread, having written y = 1, should immediately see y = 1

3) If r10 == 1 but r11 == 0, it means that another thread has not seen the entry y = 1
although x = 1 has already become visible, which is forbidden in TSO. In this case the test
goes to fail)";

const std::string rel_acq = R"(
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
    store REL #r0 r2
    store REL #r1 r2
    finish
t2: load ACQ #r1 r11
    load ACQ #r0 r10
    if r11 goto check
    finish
check: if r10 goto end
       fail
bad: fail
end: finish

-----
In TSO, the Release (REL) and Acquire (ACQ) operations provide ordering
such that if one thread sees y = 1, it is obliged to see x = 1 as well

1) TSO guarantees that if thread t2 has seen y = 1, it is bound to see x = 1 as well

2) The order store REL(x); store REL(y) must be preserved when loading load ACQ(y); load ACQ(x)

3) If r11 == 1 but r10 == 0, it means that t2 saw y = 1 but did not see x = 1, which is forbidden in TSO.
In this case the test goes to fail)";

const std::string global_store_ordering = R"(
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
check: if r10 goto end
       fail
bad: fail
end: finish

-----
In TSO, stores are done in a single global order, if one thread writes x = 1 and y = 1,
another thread cannot see y = 1 but x = 0

1) TSO guarantees that if y = 1 is visible to another thread, then x = 1 is also visible to it

2) If r11 == 1 (thread t2 saw y = 1), it is bound to see x = 1 as well

3) If r10 == 0, then the global order of records has been violated, fail is called)";

void run_program(const std::string& code_text) {
    MemoryModel model = MemoryModel::TSO;
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

TEST_CASE("Allowed Load-Store reordering in TSO") {
    std::string code_text = load_store;
    run_program(code_text);
}

TEST_CASE("FIFO behavior of the Store Buffering Passes (Store Buffering Passes)") {
    std::string code_text = fifo;
    run_program(code_text);
}

TEST_CASE("Recording order with RELEASE and ACQUIRE") {
    std::string code_text = rel_acq;
    run_program(code_text);
}

TEST_CASE("Global Store Ordering") {
    std::string code_text = global_store_ordering;
    run_program(code_text);
}
}  // namespace tso_test