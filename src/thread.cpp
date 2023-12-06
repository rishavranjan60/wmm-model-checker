#include "thread.h"

#include "code.h"
#include "defs.h"

#include <iomanip>

bool Thread::ExecNext() {
    if (IsEnd()) {
        throw RuntimeError{"Execute ended thread"};
    }
    if (static_cast<size_t>(state.rip) >= code->size()) {
        throw RuntimeError{"\"rip\" out of bounds"};
    }
    code->at(state.rip)->Evaluate(state, view.get());
    ++state.rip;
    if (state.rip == -1) {
        return is_end = true;
    }
    return false;
}

std::ostream& operator<<(std::ostream& out, const ThreadState& state) {
    out << "rip: " << state.rip << '\n';
    for (size_t i{}; const auto& reg : state.registers) {
        out << 'r' << std::setw(kDecimalDigitsInRegistersCount) << i++ << ": "; // TODO
        out << std::setw(kDecimalDigitsInWord + 1) << reg;
        out << (i % 4 == 0 ? "\n" : " | ");
    }
    return out;
}
