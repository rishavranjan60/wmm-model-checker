#include "code.h"

namespace commands {

void BinaryOperator::Evaluate(ThreadState &state) {
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

void Fai::Evaluate(ThreadState& state) {
    // TODO
}

void Cas::Evaluate(ThreadState& state) {
    // TODO
}

void If::Evaluate(ThreadState& state) {
    // TODO
}

void Fence::Evaluate(ThreadState& state) {
    // TODO
}

void Load::Evaluate(ThreadState& state) {
    // TODO
}

void Store::Evaluate(ThreadState& state) {
    // TODO
}

} // namespace commands