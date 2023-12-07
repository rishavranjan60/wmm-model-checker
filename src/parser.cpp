#include "parser.h"

#include <optional>
#include <unordered_map>

namespace {

std::unique_ptr<Command> ParseCommand(const tokens::Line& line, std::unordered_map<std::string, size_t>& labels,
                                      std::vector<std::pair<std::string, commands::If*>>& without_label,
                                      size_t line_num) {
    tokens::Token token = tokens::Constant{0};
    auto next = [&line, i = 0ull, &token](bool check_end = false) mutable {
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
        if (auto label = As<tokens::Label>(next())) {
            labels[label->name] = line_num;
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
                if (Is<tokens::Cas>(func)) {  // r0 := cas RLX #r1 r2 r3
                    auto r2 = AsThrows<tokens::Register>(next()).reg;
                    return std::make_unique<commands::Cas>(reg->reg, mem_order, mem_at_reg, r1, r2);
                }
                if (Is<tokens::Fai>(func)) {  // r0 := fai RLX #r1 r2
                    return std::make_unique<commands::Fai>(reg->reg, mem_order, mem_at_reg, r1);
                }
                throw SyntaxError{"Expected \"cas\" or \"fai\" after \":=\""};
            }
            throw SyntaxError{"Expected assigment after register"};
        }
        if (Is<tokens::If>(token)) {
            auto reg = AsThrows<tokens::Register>(next()).reg;
            auto label = AsThrows<tokens::Goto>(next()).label.name;
            auto if_st = std::make_unique<commands::If>(reg);
            if (labels.contains(label)) {
                if_st->SetLabel(labels[label]);
            } else {
                without_label.push_back({label, if_st.get()});
            }
            return if_st;
        }
        if (Is<tokens::Fence>(token)) {
            auto order = AsThrows<tokens::MemoryOrder>(next()).order;
            return std::make_unique<commands::Fence>(order);
        }
        if (Is<tokens::Load>(token) || Is<tokens::Store>(token)) {
            auto func = token;
            auto order = AsThrows<tokens::MemoryOrder>(next()).order;
            auto mem_at_reg = AsThrows<tokens::MemoryAt>(next()).reg.reg;
            auto reg = AsThrows<tokens::Register>(next()).reg;
            if (Is<tokens::Load>(func)) {
                return std::make_unique<commands::Load>(order, mem_at_reg, reg);
            }
            return std::make_unique<commands::Store>(order, mem_at_reg, reg);
        }
        if (Is<tokens::Finish>(token)) {
            return std::make_unique<commands::Finish>();
        }
      if (Is<tokens::Fail>(token)) {
          return std::make_unique<commands::Fail>();
      }
        throw SyntaxError{"Unknown command"};
    }();
    next(true);
    return res;
}

}  // namespace

Code Parse(Tokenizer& tokenizer) {
    Code result;
    std::unordered_map<std::string, size_t> labels;
    std::vector<std::pair<std::string, commands::If*>> without_label;
    for (size_t line{0}; !tokenizer.IsEnd(); ++line) {
        try {
            result.push_back(ParseCommand(tokenizer.GetLine(), labels, without_label, line));
        } catch (SyntaxError& syntax_error) {
            throw SyntaxError{"Line " + std::to_string(line + 1) + ": " + syntax_error.what()};
        }
        tokenizer.Next();
    }
    for (auto& [label, if_st] : without_label) {
        if (!labels.contains(label)) {
            throw SyntaxError{"Label \"" + label + "\" doesn't exist"};
        }
        if_st->SetLabel(labels[label]);
    }
    return result;
}