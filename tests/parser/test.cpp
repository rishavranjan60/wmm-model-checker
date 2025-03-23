#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>

#define TESTING
#include "errors.h"
#include "parser.h"

namespace
{

template <class T> T *As(Command *cmd)
{
    auto *res = dynamic_cast<T *>(cmd);
    REQUIRE(res);
    return res;
}

Code GetCode(std::string str, size_t lines_count = 1)
{
    std::stringstream ss{std::move(str)};
    Tokenizer tokenizer{ss};
    auto code = Parse(tokenizer);
    REQUIRE(lines_count == code.size());
    return code;
}

TEST_CASE("Assigment number")
{
    auto lines = GetCode("r3 = 42\n\t"
                         "r0 = 1234567890",
                         2);
    auto *assigment = As<commands::Assigment>(lines[0].get());
    auto *rhs = As<commands::Number>(assigment->rhs.get());
    REQUIRE(assigment->lhs == Register{3});
    REQUIRE(rhs->value == 42);
    assigment = As<commands::Assigment>(lines[1].get());
    rhs = As<commands::Number>(assigment->rhs.get());
    REQUIRE(assigment->lhs == Register{0});
    REQUIRE(rhs->value == 1234567890);

    REQUIRE_THROWS_WITH(GetCode("r1 = cas"), "Line 1: Expected number or operator after \"=\"");

    lines = GetCode("bruh: r0 = -5");
    assigment = As<commands::Assigment>(lines[0].get());
    rhs = As<commands::Number>(assigment->rhs.get());
    REQUIRE(assigment->lhs == Register{0});
    REQUIRE(rhs->value == -5);
}

TEST_CASE("Assigment operator")
{
    auto lines = GetCode("r5 = + r0 r1\n\t"
                         "r11 = - r4 r4\n   "
                         "r2 = * r2 r14\n\n\n"
                         "r14 = / r6 r9\t\n"
                         "r15 = ^ r0 r1",
                         5);
    auto make_data = [](Register lhs, ::BinaryOperator op, Register l, Register r)
    { return std::make_tuple(lhs, op, l, r); };
    auto test_data = {
        make_data(5, ::BinaryOperator::PLUS, 0, 1), make_data(11, ::BinaryOperator::MINUS, 4, 4),
        make_data(2, ::BinaryOperator::MULTIPLY, 2, 14),
        make_data(14, ::BinaryOperator::DIVIDE, 6, 9), make_data(15, ::BinaryOperator::XOR, 0, 1)};
    for (size_t i{}; const auto &[lhs, op, l, r] : test_data)
    {
        auto *assigment = As<commands::Assigment>(lines[i++].get());
        auto *rhs = As<commands::BinaryOperator>(assigment->rhs.get());
        REQUIRE(assigment->lhs == lhs);
        REQUIRE(rhs->op == op);
        REQUIRE(rhs->lhs == l);
        REQUIRE(rhs->rhs == r);
    }

    REQUIRE_THROWS_WITH(GetCode("r0 = + r0 r0 \n r2 = * r2"), "Line 2: Expected more tokens");
    REQUIRE_THROWS_WITH(GetCode("r0 = + r0 r0 r0"), "Line 1: Expected 5 tokens, found 6");
    REQUIRE_THROWS_AS(GetCode("r0 = / r11 r12 r13"), SyntaxError);
}

TEST_CASE("Cas")
{
    auto lines = GetCode("r2 := cas RLX #r10 r0 r1");
    auto *cas = As<commands::Cas>(lines[0].get());
    REQUIRE(cas->res == Register{2});
    REQUIRE(cas->order == MemoryOrder::RLX);
    REQUIRE(cas->at == Register{10});
    REQUIRE(cas->expected == Register{0});
    REQUIRE(cas->desired == Register{1});

    REQUIRE_THROWS_AS(GetCode("r1 := r2"), SyntaxError);
    REQUIRE_THROWS_WITH(GetCode("r1 r2"), "Line 1: Expected assigment after register");
}

TEST_CASE("Fai")
{
    auto lines = GetCode("r0 := fai SEQ_CST #r0 r2");
    auto *cas = As<commands::Fai>(lines[0].get());
    REQUIRE(cas->res == Register{0});
    REQUIRE(cas->order == MemoryOrder::SEQ_CST);
    REQUIRE(cas->at == Register{0});
    REQUIRE(cas->add == Register{2});

    REQUIRE_THROWS_AS(GetCode("r1 := fai RLX r1 r2"), SyntaxError);
    REQUIRE_THROWS_WITH(GetCode("\n\n\nr2 := RLX RLX #r1 r2"),
                        "Line 1: Expected \"cas\" or \"fai\" after \":=\"");
}

TEST_CASE("If")
{
    auto lines = GetCode("asd: r0 = 1\n"
                         "if r15 goto asd",
                         2);
    auto *if_st = As<commands::If>(lines[1].get());
    REQUIRE(if_st->condition == Register{15});
    REQUIRE(if_st->cmd_num == 0);

    lines = GetCode("if r15 goto asd\n"
                    "asd: r0 = 1\n",
                    2);
    if_st = As<commands::If>(lines[0].get());
    REQUIRE(if_st->condition == Register{15});
    REQUIRE(if_st->cmd_num == 1);

    REQUIRE_THROWS_AS(GetCode("if r4 goto asd"), SyntaxError);
    REQUIRE_THROWS_WITH(GetCode("if r4 goto asd"), "Label \"asd\" doesn't exist");
}

TEST_CASE("Fence")
{
    auto lines = GetCode("fence RLX\n"
                         "fence SEQ_CST\n"
                         "fence ACQ\n",
                         3);
    auto *fence = As<commands::Fence>(lines[0].get());
    REQUIRE(fence->order == MemoryOrder::RLX);
    fence = As<commands::Fence>(lines[1].get());
    REQUIRE(fence->order == MemoryOrder::SEQ_CST);
    fence = As<commands::Fence>(lines[2].get());
    REQUIRE(fence->order == MemoryOrder::ACQ);

    REQUIRE_THROWS_AS(GetCode("fence"), SyntaxError);
}

TEST_CASE("Store/Load")
{
    auto lines = GetCode("aboba: store RLX #r1 r0\n"
                         "load ACQ #r13 r9\n"
                         "store \t\tSEQ_CST #r0 r0",
                         3);
    auto *store = As<commands::Store>(lines[0].get());
    REQUIRE(store->order == MemoryOrder::RLX);
    REQUIRE(store->at == Register{1});
    REQUIRE(store->reg == Register{0});
    auto *load = As<commands::Load>(lines[1].get());
    REQUIRE(load->order == MemoryOrder::ACQ);
    REQUIRE(load->at == Register{13});
    REQUIRE(load->reg == Register{9});
    store = As<commands::Store>(lines[2].get());
    REQUIRE(store->order == MemoryOrder::SEQ_CST);
    REQUIRE(store->at == Register{0});
    REQUIRE(store->reg == Register{0});

    REQUIRE_THROWS_AS(GetCode("store load"), SyntaxError);
    REQUIRE_THROWS_AS(GetCode("store RLX r0 r0"), SyntaxError);
    REQUIRE_THROWS_AS(GetCode("load RLX #r0 r0 r0"), SyntaxError);
}

TEST_CASE("Finish/Fail")
{
    auto lines = GetCode("finish\nfail", 2);
    As<commands::Finish>(lines[0].get());
    As<commands::Fail>(lines[1].get());
}

TEST_CASE("Bad")
{
    REQUIRE_THROWS_WITH(GetCode("#r1"), "Line 1: Unknown command");
    REQUIRE_THROWS_AS(GetCode("fail fail"), SyntaxError);
}

} // namespace