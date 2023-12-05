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
    void Evaluate(ThreadState& state) override { state.registers.back() = value; }
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
    void Evaluate(ThreadState & state) override;
};

CLASS Fai : public Command {
    Register res;
    MemoryOrder order;
    Register at;
    Register add;

public:
    Fai(Register res, MemoryOrder order, Register at, Register add) : res(res), order(order), at(at), add(add) {}
    void Evaluate(ThreadState & state) override;
};

CLASS Cas : public Command {
    Register res;
    MemoryOrder order;
    Register at;
    Register expected;
    Register desired;

public:
    Cas(Register res, MemoryOrder mode, Register at, Register expected, Register desired)
        : res(res), order(mode), at(at), expected(expected), desired(desired) {}
    void Evaluate(ThreadState & state) override;
};

CLASS If : public Command {
    Register condition;
    size_t cmd_num;

public:
    If(Register condition) : condition(condition) {}
    void Evaluate(ThreadState & state) override;

    void SetLabel(size_t num) { cmd_num = num; }
};

CLASS Fence : public Command {
    MemoryOrder order;

public:
    Fence(MemoryOrder order) : order(order) {}
    void Evaluate(ThreadState & state) override;
};

CLASS Load : public Command {
    MemoryOrder order;
    Register at;
    Register reg;

public:
    Load(MemoryOrder order, Register at, Register reg) : order(order), at(at), reg(reg) {}
    void Evaluate(ThreadState & state) override;
};

CLASS Store : public Command {
    MemoryOrder order;
    Register at;
    Register reg;

public:
    Store(MemoryOrder order, Register at, Register reg) : order(order), at(at), reg(reg) {}
    void Evaluate(ThreadState & state) override;
};

}  // namespace commands

#undef CLASS