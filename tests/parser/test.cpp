#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>

#define TESTING
#include "parser.h"
#include "errors.h"

using namespace commands;

namespace {

template<class T>
T* As(Command* cmd) {
    auto* res = dynamic_cast<T*>(cmd);
    REQUIRE(res);
    return res;
}

Code GetCode(std::string str, size_t lines_count = 1) {
    std::stringstream ss{std::move(str)};
    Tokenizer tokenizer{ss};
    auto code = Parse(tokenizer);
    REQUIRE(lines_count == code.size());
    return code;
}

TEST_CASE("Assigment number") {
    auto lines = GetCode(
        "r3 = 42\n\t"
        "r0 = 1234567890", 2);
    auto* assigment = As<Assigment>(lines[0].get());
    auto* rhs = As<Number>(assigment->rhs.get());
    REQUIRE(assigment->lhs == Register{3});
    REQUIRE(rhs->value == 42);
    assigment = As<Assigment>(lines[1].get());
    rhs = As<Number>(assigment->rhs.get());
    REQUIRE(assigment->lhs == Register{0});
    REQUIRE(rhs->value == 1234567890);

    REQUIRE_THROWS_WITH(GetCode("r1 = cas"), "Line 1: Expected number or operator after \"=\"");

    lines = GetCode("bruh: r0 = -5"); // TODO

}

TEST_CASE("Assigment operator") {
    auto lines = GetCode(
        "r5 = + r0 r1\n\t"
        "r11 = - r4 r4\n   "
        "r2 = * r2 r14\n\n\n"
        "r14 = / r6 r9", 4);
    auto make_data = [](Register lhs, ::BinaryOperator op, Register l, Register r) {
        return std::make_tuple(lhs, op, l, r);
    };
    auto test_data = {
        make_data(5, ::BinaryOperator::PLUS, 0, 1),
        make_data(11, ::BinaryOperator::MINUS, 4, 4),
        make_data(2, ::BinaryOperator::MULTIPLY, 2, 14),
        make_data(14, ::BinaryOperator::DIVIDE, 6, 9)
    };
    for (size_t i{}; const auto& [lhs, op, l, r] : test_data) {
        auto* assigment = As<Assigment>(lines[i++].get());
        auto* rhs = As<commands::BinaryOperator>(assigment->rhs.get());
        REQUIRE(assigment->lhs == lhs);
        REQUIRE(rhs->op == op);
        REQUIRE(rhs->lhs == l);
        REQUIRE(rhs->rhs == r);
    }

    REQUIRE_THROWS_WITH(GetCode("r0 = + r0 r0 \n r2 = * r2"), "Line 2: Expected more tokens");
    REQUIRE_THROWS_WITH(GetCode("r0 = + r0 r0 r0"), "Line 1: Expected 5 tokens, found 6");
    REQUIRE_THROWS_AS(GetCode("r0 = / r11 r12 r13"), SyntaxError);
}

TEST_CASE("Cas") {
    auto lines = GetCode("r2 := cas RLX #r10 r0 r1");
    auto* cas = As<commands::Cas>(lines[0].get());
    REQUIRE(cas->res == Register{2});
    REQUIRE(cas->order == MemoryOrder::RLX);
    REQUIRE(cas->at == Register{10});
    REQUIRE(cas->expected == Register{0});
    REQUIRE(cas->desired == Register{1});

    REQUIRE_THROWS_AS(GetCode("r1 := r2"), SyntaxError);
    REQUIRE_THROWS_WITH(GetCode("r1 r2"), "Line 1: Expected assigment after register");
}

TEST_CASE("Fai") {
    auto lines = GetCode("r0 := fai SEQ_CST #r0 r2");
    auto* cas = As<commands::Fai>(lines[0].get());
    REQUIRE(cas->res == Register{0});
    REQUIRE(cas->order == MemoryOrder::SEQ_CST);
    REQUIRE(cas->at == Register{0});
    REQUIRE(cas->add == Register{2});

    REQUIRE_THROWS_AS(GetCode("r1 := fai RLX r1 r2"), SyntaxError);
    REQUIRE_THROWS_WITH(GetCode("\n\n\nr2 := RLX RLX #r1 r2"), "Line 1: Expected \"cas\" or \"fai\" after \":=\"");
}

} // namespace