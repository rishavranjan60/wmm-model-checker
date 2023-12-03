#pragma once

#include "thread.h"
#include "defs.h"

#include <vector>
#include <memory>

#ifdef TESTING
#define CLASS struct
#else
#define CLASS class
#endif

class Command {
public:
    virtual void Evaluate(ThreadState& state) = 0;

    virtual ~Command() = default;
};

using Code = std::vector<std::unique_ptr<Command>>;

namespace commands {

CLASS Number : public Command {
    Word value;

public:
    Number(Word value) : value(value) {}

    void Evaluate(ThreadState & state) override { state.registers.back() = value; }
};

CLASS Assigment : public Command {
    Register lhs;
    std::unique_ptr<Command> rhs;

public:
    Assigment(Register lhs, std::unique_ptr<Command> rhs) : lhs(lhs), rhs(std::move(rhs)) {}

    void Evaluate(ThreadState & state) override {
        rhs->Evaluate(state);
        state.registers[lhs] = state.registers.back();
    }
};

CLASS BinaryOperator : public Command {
    Register lhs;
    Register rhs;
    ::BinaryOperator op;

public:
    BinaryOperator(Register lhs, Register rhs, ::BinaryOperator op) : lhs(lhs), rhs(rhs), op(op) {}

    void Evaluate(ThreadState & state) override {
        const auto& l = state.registers[lhs];
        const auto& r = state.registers[rhs];
        auto& res = state.registers.back();
        switch (op) {
            case ::BinaryOperator::PLUS:
                res = l + r;
                break;
            case ::BinaryOperator::MINUS:
                res = l - r;
                break;
            case ::BinaryOperator::MULTIPLY:
                res = l * r;
                break;
            case ::BinaryOperator::DIVIDE:
                res = l / r;
                break;
        }
    }
};

}  // namespace commands

#undef CLASS