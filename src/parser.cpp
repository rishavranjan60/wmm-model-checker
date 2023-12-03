#include "parser.h"

#include <optional>

namespace {

std::unique_ptr<Command> ParseCommand(const tokens::Line& line) {
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
    auto get_register = [&](tokens::Token token) -> std::optional<Register> {
        if (auto reg = std::get_if<tokens::Register>(&token)) {
            return {reg->reg};
        }
        return {};
    };
    auto res = [&]() {
        using tokens::Is;
        if (auto reg = get_register(next()); reg) {
            if (Is<tokens::Assigment>(next())) {
                if (Is<tokens::Constant>(next())) {  // r0 = 42
                    return std::make_unique<commands::Assigment>(
                        *reg, std::make_unique<commands::Number>(std::get<tokens::Constant>(token).value));
                }
                if (Is<tokens::BinaryOperator>(token)) {  // r0 = + 40 2
                    auto op = std::get<tokens::BinaryOperator>(token).op;
                    auto lhs = get_register(next());
                    auto rhs = get_register(next());
                    if (lhs && rhs) {
                        return std::make_unique<commands::Assigment>(
                            *reg, std::make_unique<commands::BinaryOperator>(*lhs, *rhs, op));
                    }
                    throw SyntaxError{"Expected registers as arguments"};
                }
                throw SyntaxError{"Expected number or operator after \"=\""};
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
    for (size_t line{1}; !tokenizer.IsEnd(); ++line) {
        try {
            result.push_back(ParseCommand(tokenizer.GetLine()));
        } catch (SyntaxError& syntax_error) {
            throw SyntaxError{"Line " + std::to_string(line) + ": " + syntax_error.what()};
        }
        tokenizer.Next();
    }
    return result;
}