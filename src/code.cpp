#include "code.h"

namespace commands {

void BinaryOperator::Evaluate(ThreadState &state, MemoryView*) {
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

void Fai::Evaluate(ThreadState& state, MemoryView* view) {
    auto prev = view->Load(state[at], order);
    view->Store(state[at], prev + state[add], order);
    state[res] = prev;
}

void Cas::Evaluate(ThreadState& state, MemoryView* view) {
    auto prev = view->Load(state[at], order);
    if (prev == state[expected]) {
        std::swap(state[at], state[desired]);
    }
    state[res] = prev;
}

void If::Evaluate(ThreadState& state, MemoryView*) {
    if (state[condition] != 0) {
        state.rip = cmd_num;
        --state.rip;
    }
}

void Fence::Evaluate(ThreadState&, MemoryView* view) {
    view->Fence(order);
}

void Load::Evaluate(ThreadState& state, MemoryView* view) {
    state[reg] = view->Load(state[at], order);
}

void Store::Evaluate(ThreadState& state, MemoryView* view) {
    view->Store(state[at], state[reg], order);
}

} // namespace commands