#include "thread.h"

#include "code.h"
#include "defs.h"

#include <iomanip>

bool Thread::ExecNext() {
    if (IsEnd()) {
        throw RuntimeError{"Execute ended thread"};
    }
    if (view->HasSilent()) {
        if (path_chooser->ExecSilent()) {
            view->DoSilent();
            return false;
        }
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

void ThreadState::Print(std::ostream& out) const {
    out << "rip: " << rip << '\n';
    for (size_t i{}; const auto& reg : registers) {
        out << 'r' << std::setw(kDecimalDigitsInRegistersCount) << std::left << i++ << ": ";
        out << std::setw(kDecimalDigitsInWord + 1) << reg;
        out << (i % 4 == 0 ? "\n" : " | ");
    }
}
