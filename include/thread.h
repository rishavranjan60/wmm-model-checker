#pragma once

#include "defs.h"
#include "errors.h"
#include "mem_models/memory_system.h"
#include "path_choosers/path_chooser.h"

#include <array>
#include <cstdint>
#include <cstddef>
#include <memory>
#include <vector>
#include <iostream>

class Command;
using Code = std::vector<std::unique_ptr<Command>>;

struct ThreadState {
    // r15 - store result of last arithmetic operation or last constant
    std::array<Word, kRegistersCount> registers;
    Word rip;

    Word operator[](Register reg) const {
        CheckReg(reg);
        return registers[reg];
    }

    Word& operator[](Register reg) {
        CheckReg(reg);
        return registers[reg];
    }

    Word& AuxReg() { return registers.back(); }

    void Print(std::ostream&) const;

private:
    void CheckReg(Register reg) const {
        if (static_cast<size_t>(reg) >= kRegistersCount) {
            throw std::logic_error{"ThreadState: register out of bounds"};
        }
    }
};

class Thread {
private:
    ThreadState state;
    const Code* code;
    bool is_end = false;

    std::unique_ptr<MemoryView> view;

    std::shared_ptr<PathChooser> path_chooser;

public:
    Thread(const Code* code, std::unique_ptr<MemoryView> view, std::shared_ptr<PathChooser> path_chooser, size_t thread_id)
        : state{{}, 0}, code{code}, view{std::move(view)}, path_chooser{std::move(path_chooser)} {
        state.AuxReg() = thread_id;
    }

    bool IsEnd() const { return is_end; }

    bool ExecNext();
    const ThreadState& GetState() const { return state; }
    void PrintMemView(std::ostream& out = std::cout) const {
        view->Print(out);
    }
};