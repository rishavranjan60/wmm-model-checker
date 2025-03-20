#include <catch2/catch_test_macros.hpp>
#include "defs.h"

#include <parser.h>
#include <program.h>
#include <tokenizer.h>
#include <path_choosers/full.h>

const std::string kDekkerLockProgram = R"(
r0 = + r1 r15
r1 = 1
r2 = 1
r3 = 5
store REL #r3 r2
if r0 goto t2
if r1 goto t1
fail
t1: r0 = 0
store REL #r0 r2
load ACQ #r1 r10
store REL #r3 r10
finish
t2: r0 = 0
store REL #r1 r2
load ACQ #r0 r11
load ACQ #r3 r10
if r10 goto end
if r11 goto end
fail
end: finish

-----
      z = 1
    x = y = 0
x   = 1 | y   = 1
r10 = y | r11 = x
z = r10 | assert not z == 0 && r11 == 0
)";

const std::string kMessagePassingProgram = R"(
r0 = + r0 r15
r1 = 1
if r0 goto t2+
if r1 goto t1
t2+: r0 = - r0 r1
if r0 goto bad
if r1 goto t2
t1: r0 = 0
r1 = 1
load ACQ #r1 r11
load RLX #r0 r10
if r11 goto check
finish
check: if r10 goto end
bad: fail
t2: r0 = 0
r1 = 1
r2 = 1
store RLX #r0 r2
store REL #r1 r2
end: finish

-----
    x = y = 0
r11 = y (ACQ) | x = 1 (RLX)
r10 = x (RLX) | y = 1 (REL)
assert not r10 == 0 && r11 == 1 | skip)";

TEST_CASE("Dekker lock fails under RA") {
    constexpr auto kModel = MemoryModel::RA;
    constexpr std::size_t kThreadsCount = 2;
    std::stringstream ss(kDekkerLockProgram);
    Tokenizer tokenizer{ss};
    auto code = Parse(tokenizer);
    auto checker = std::make_shared<FullChooser>();
    Program program(std::move(code), kThreadsCount, checker);
    try {
        while (!checker->Finished()) {
            constexpr std::size_t kMemorySize = 8;
            program.Init(kModel, kMemorySize);
            program.SetVerbosity(false);
            program.Run();
            checker->NextRun();
        }
        REQUIRE(false);
    } catch (FailError&) {
    }
}

TEST_CASE("Message passing works") {
    constexpr auto kModel = MemoryModel::RA;
    constexpr std::size_t kThreadsCount = 2;
    std::stringstream ss(kMessagePassingProgram);
    Tokenizer tokenizer{ss};
    auto code = Parse(tokenizer);
    auto checker = std::make_shared<FullChooser>();
    Program program(std::move(code), kThreadsCount, checker);
    try {
        while (!checker->Finished()) {
            constexpr std::size_t kMemorySize = 8;
            program.Init(kModel, kMemorySize);
            program.SetVerbosity(false);
            program.Run();
            checker->NextRun();
        }
    } catch (FailError&) {
        // fail in case there was 'fail' instruction reached
        REQUIRE(false);
    }
}
