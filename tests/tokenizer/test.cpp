#include <catch2/catch_test_macros.hpp>

#include "tokenizer.h"

namespace
{

template <class T> void RequireEquals(const tokens::Token &token, const T &value)
{
    if (const auto *p = std::get_if<T>(&token))
    {
        REQUIRE(*p == value);
    }
    else
    {
        FAIL("Wrong token type: " << typeid(T).name());
    }
}

void CheckTokens(const tokens::Line &line, const auto &...tokens)
{
    auto check_token = [&line, i = 0](const auto &token) mutable
    { RequireEquals(line[i++], token); };
    (check_token(tokens), ...);
}

std::vector<tokens::Line> GetLines(std::string str, size_t lines_count = 1)
{
    std::stringstream ss{std::move(str)};
    Tokenizer tokenizer{ss};
    std::vector<tokens::Line> res;
    while (!tokenizer.IsEnd())
    {
        res.push_back(tokenizer.GetLine());
        tokenizer.Next();
    }
    REQUIRE(lines_count == res.size());
    return res;
}

TEST_CASE("One token")
{
    auto lines = GetLines("r0");
    CheckTokens(lines[0], tokens::Register{0});
    lines = GetLines("   =   ");
    CheckTokens(lines[0], tokens::Assigment{});
    lines = GetLines("* ");
    CheckTokens(lines[0], tokens::BinaryOperator{::BinaryOperator::MULTIPLY});
    lines = GetLines("\n\n^\n");
    CheckTokens(lines[0], tokens::BinaryOperator{::BinaryOperator::XOR});
    lines = GetLines(" goto bimbimbambam");
    CheckTokens(lines[0], tokens::Goto{{"bimbimbambam"}});
    lines = GetLines("cas    ");
    CheckTokens(lines[0], tokens::Cas{});
    lines = GetLines("   :=       ");
    CheckTokens(lines[0], tokens::ReturnAssigment{});
    lines = GetLines("label: ");
    CheckTokens(lines[0], tokens::Label{"label"});
    lines = GetLines("#r10");
    CheckTokens(lines[0], tokens::MemoryAt{tokens::Register{10}});
    lines = GetLines("finish");
    CheckTokens(lines[0], tokens::Finish{});
    lines = GetLines("fail");
    CheckTokens(lines[0], tokens::Fail{});
}

TEST_CASE("One line")
{
    auto lines = GetLines("r0 = 42");
    CheckTokens(lines[0], tokens::Register{0}, tokens::Assigment{}, tokens::Constant{42});
    lines = GetLines("r10 = r3 * r15");
    CheckTokens(lines[0], tokens::Register{10}, tokens::Assigment{}, tokens::Register{3},
                tokens::BinaryOperator{::BinaryOperator::MULTIPLY}, tokens::Register{15});
    lines = GetLines("if r5 goto aboba");
    CheckTokens(lines[0], tokens::If{}, tokens::Register{5}, tokens::Goto{{"aboba"}});
    lines = GetLines("pupupu: r1 := cas RLX #r2 r3 r4");
    CheckTokens(lines[0], tokens::Label{"pupupu"}, tokens::Register{1}, tokens::ReturnAssigment{},
                tokens::Cas{}, tokens::MemoryOrder{::MemoryOrder::RLX},
                tokens::MemoryAt{tokens::Register{2}}, tokens::Register{3}, tokens::Register{4});
}

TEST_CASE("Couple lines")
{
    auto lines = GetLines("fence REL_ACQ\n\t r4 := fai ACQ #r0 r9\n  finish\n\n\nfail", 4);
    CheckTokens(lines[0], tokens::Fence{}, tokens::MemoryOrder{::MemoryOrder::REL_ACQ});
    CheckTokens(lines[1], tokens::Register{4}, tokens::ReturnAssigment{}, tokens::Fai{},
                tokens::MemoryOrder{::MemoryOrder::ACQ}, tokens::MemoryAt{tokens::Register{0}},
                tokens::Register{9});
    CheckTokens(lines[2], tokens::Finish{});
    CheckTokens(lines[3], tokens::Fail{});
}

} // namespace
