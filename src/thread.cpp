#include "thread.h"

#include "code.h"
#include "defs.h"

#include <iomanip>

bool Thread::ExecNext() {
    if (IsEnd()) {
        throw RuntimeError{"Execute ended thread"};
    }
    if (view->HasSilent()) {
        static const std::vector<std::string> kVariants = { "non-silent step", "silent step"};
        if (path_chooser->ChooseVariant(kVariants, "step type")) {
            view->DoSilent();
            return false;
        }
    }
    while (true) {
        if (static_cast<size_t>(state.rip) >= code->size()) {
            throw RuntimeError{"\"rip\" out of bounds"};
        }
        auto& cmd = code->at(state.rip);
        bool is_silent = dynamic_cast<ThreadSilentCommand*>(cmd.get());
        cmd->Evaluate(state, view.get());
        ++state.rip;
        if (state.rip == -1) {
            return is_end = true;
        }
        if (!skip_silent || !is_silent) {
            return false;
        }
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
