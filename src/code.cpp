#include "code.h"

namespace commands {

void BinaryOperator::Evaluate(ThreadState& state, MemoryView*) {
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
            if (r == 0) {
                throw RuntimeError{"Divide by zero"};
            }
            res = l / r;
            break;
        case ::BinaryOperator::XOR:
            res = l ^ r;
            break;
        default:
            throw std::logic_error{"Unimplemented binary operator"};
    }
}

void Fai::Evaluate(ThreadState& state, MemoryView* view) { state[res] = view->Fai(state[at], state[add], order); }

void Cas::Evaluate(ThreadState& state, MemoryView* view) {
    state[res] = view->Cas(state[at], state[expected], state[desired], order);
}

void If::Evaluate(ThreadState& state, MemoryView*) {
    if (state[condition] != 0) {
        state.rip = cmd_num;
        --state.rip;
    }
}

void Fence::Evaluate(ThreadState&, MemoryView* view) { view->Fence(order); }

void Load::Evaluate(ThreadState& state, MemoryView* view) { state[reg] = view->Load(state[at], order); }

void Store::Evaluate(ThreadState& state, MemoryView* view) { view->Store(state[at], state[reg], order); }

}  // namespace commands