#include "parser.h"

#include <optional>
#include <unordered_map>

namespace {

std::unique_ptr<Command> ParseCommand(const tokens::Line& line, std::unordered_map<std::string, size_t>& labels) {
    tokens::Token token = tokens::Constant{0};
    auto next = [&line, i = 0, &token](bool check_end = false) mutable {
        if (check_end) {
            if (i < line.size()) {
                throw SyntaxError{"Expected " + std::to_string(i) + " tokens, found " + std::to_string(line.size())};
            }
            return token;
        }
        if (i >= line.size()) {
            throw SyntaxError{"Expected more tokens"};
        }
        return token = line[i++];
    };
    auto res = [&]() -> std::unique_ptr<Command> {
        using tokens::Is;
        using tokens::AsThrows;
        if (Is<tokens::Label>(next())) {
            // TODO
            next();
        }
        if (auto reg = As<tokens::Register>(token)) {
            if (Is<tokens::Assigment>(next())) {
                if (auto number = As<tokens::Constant>(next())) {  // r0 = 42
                    return std::make_unique<commands::Assigment>(reg->reg,
                                                                 std::make_unique<commands::Number>(number->value));
                }
                if (auto bin_op = As<tokens::BinaryOperator>(token)) {  // r0 = + r0 r1
                    auto lhs = AsThrows<tokens::Register>(next()).reg;
                    auto rhs = AsThrows<tokens::Register>(next()).reg;
                    return std::make_unique<commands::Assigment>(
                        reg->reg, std::make_unique<commands::BinaryOperator>(lhs, rhs, bin_op->op));
                }
                throw SyntaxError{"Expected number or operator after \"=\""};
            }
            if (Is<tokens::ReturnAssigment>(token)) {
                auto func = next();
                auto mem_order = AsThrows<tokens::MemoryOrder>(next()).order;
                auto mem_at_reg = AsThrows<tokens::MemoryAt>(next()).reg.reg;
                auto r1 = AsThrows<tokens::Register>(next()).reg;
                if (Is<tokens::Cas>(func)) {  // todo: example
                    auto r2 = AsThrows<tokens::Register>(next()).reg;
                    return std::make_unique<commands::Cas>(reg->reg, mem_order, mem_at_reg, r1, r2);
                }
                if (Is<tokens::Fai>(func)) {  // TODO: example
                    return std::make_unique<commands::Fai>(reg->reg, mem_order, mem_at_reg, r1);
                }
                throw SyntaxError{"Expected \"cas\" or \"fai\" after \":=\""};
            }
            throw SyntaxError{"Expected assigment after register"};
        }
    }();
    next(true);
    return res;
}

}  // namespace

Code Parse(Tokenizer& tokenizer) {
    Code result;
    std::unordered_map<std::string, size_t> labels;
    for (size_t line{1}; !tokenizer.IsEnd(); ++line) {
        try {
            result.push_back(ParseCommand(tokenizer.GetLine(), labels));
        } catch (SyntaxError& syntax_error) {
            throw SyntaxError{"Line " + std::to_string(line) + ": " + syntax_error.what()};
        }
        tokenizer.Next();
    }
    return result;
}