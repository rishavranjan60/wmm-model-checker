#pragma once

#include "thread.h"
#include "defs.h"
#include "memory_system/memory_view.h"

#include <vector>
#include <memory>

#ifdef TESTING
#define CLASS struct
#else
#define CLASS class
#endif

class Command {
public:
    virtual void Evaluate(ThreadState &state, MemoryView *mem_view) = 0;

    virtual ~Command() = default;
};

using Code = std::vector<std::unique_ptr<Command>>;

class ThreadSilentCommand : public Command {};

namespace commands {

CLASS Number : public ThreadSilentCommand {
    Word value;

public:
    Number(Word value) : value(value) {}
    void Evaluate(ThreadState & state, MemoryView *) override { state.AuxReg() = value; }
};

CLASS Assigment : public ThreadSilentCommand {
    Register lhs;
    std::unique_ptr<Command> rhs;

public:
    Assigment(Register lhs, std::unique_ptr<Command> rhs) : lhs(lhs), rhs(std::move(rhs)) {}
    void Evaluate(ThreadState & state, MemoryView * view) override {
        rhs->Evaluate(state, view);
        state[lhs] = state.AuxReg();
    }
};

CLASS BinaryOperator : public ThreadSilentCommand {
    Register lhs;
    Register rhs;
    ::BinaryOperator op;

public:
    BinaryOperator(Register lhs, Register rhs, ::BinaryOperator op) : lhs(lhs), rhs(rhs), op(op) {}
    void Evaluate(ThreadState &, MemoryView *) override;
};

CLASS Fai : public Command {
    Register res;
    MemoryOrder order;
    Register at;
    Register add;

public:
    Fai(Register res, MemoryOrder order, Register at, Register add) : res(res), order(order), at(at), add(add) {}
    void Evaluate(ThreadState &, MemoryView *) override;
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
    void Evaluate(ThreadState &, MemoryView *) override;
};

CLASS If : public ThreadSilentCommand {
    Register condition;
    size_t cmd_num;

public:
    If(Register condition) : condition(condition) {}
    void Evaluate(ThreadState &, MemoryView *) override;

    void SetLabel(size_t num) { cmd_num = num; }
};

CLASS Fence : public Command {
    MemoryOrder order;

public:
    Fence(MemoryOrder order) : order(order) {}
    void Evaluate(ThreadState &, MemoryView *) override;
};

CLASS Load : public Command {
    MemoryOrder order;
    Register at;
    Register reg;

public:
    Load(MemoryOrder order, Register at, Register reg) : order(order), at(at), reg(reg) {}
    void Evaluate(ThreadState &, MemoryView *) override;
};

CLASS Store : public Command {
    MemoryOrder order;
    Register at;
    Register reg;

public:
    Store(MemoryOrder order, Register at, Register reg) : order(order), at(at), reg(reg) {}
    void Evaluate(ThreadState &, MemoryView *) override;
};

CLASS Finish : public Command {
    struct Stub {};

public:
    void Evaluate(ThreadState & state, MemoryView * view) override {
        view->Fence(MemoryOrder::SEQ_CST);
        state.rip = -2;
    }
};

CLASS Fail : public Command {
    struct Stub {};

public:
    void Evaluate(ThreadState & state, MemoryView *) override { throw FailError{"rip: " + std::to_string(state.rip)}; }
};

}  // namespace commands

#undef CLASS