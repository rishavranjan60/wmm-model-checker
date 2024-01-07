#include "thread.h"

#include "code.h"
#include "defs.h"

#include <iomanip>

bool Thread::ExecNext() {
    if (IsEnd()) {
        throw RuntimeError{"Execute ended thread"};
    }
    if (view->HasSilent()) {
        if (path_chooser->ChooseVariant({"non-silent step", "silent step"}, "step type")) {
            view->DoSilent();
            return false;
        }
    }
    return Execute();
}

bool Thread::Execute() {
    while (true) {
        if (static_cast<size_t>(state.rip) >= code->size()) {
            throw RuntimeError{"\"rip\" out of bounds"};
        }
        auto& cmd = code->at(state.rip);
        cmd->Evaluate(state, view.get());
        ++state.rip;
        if (state.rip == -1) {
            return is_end = true;
        }
        bool is_silent = dynamic_cast<ThreadSilentCommand*>(code->at(state.rip).get());
        if (!skip_silent || !is_silent) {
            return false;
        }
    }
}

void ThreadState::Print(std::ostream& out) const {
    out << "rip: " << rip << '\n';
    for (size_t i{}; const auto& reg : registers) {
        out << 'r' << std::setw(kDecimalDigitsInRegistersCount) << std::left << i++ << ": ";
        out << std::setw(kDecimalDigitsInWord + 1) << reg;
        out << (i % 4 == 0 ? "\n" : " | ");
    }
}

void Thread::PrintCurrentCommand(std::ostream& out) const {
    code->at(state.rip)->Print(out);
    out << '\n';
}