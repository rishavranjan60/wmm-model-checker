#pragma once

#include "defs.h"
#include "errors.h"
#include "memory_system.h"

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

private:
    void CheckReg(Register reg) const {
        if (static_cast<size_t>(reg) >= kRegistersCount) {
            throw std::logic_error{"ThreadState: register out of bounds"};
        }
    }
};

std::ostream& operator<<(std::ostream&, const ThreadState&);

class Thread {
private:
    ThreadState state;
    const Code* code;
    bool is_end = false;

    std::unique_ptr<MemoryView> view;

public:
    Thread(const Code* code, std::unique_ptr<MemoryView> view, Word rip = 0)
        : state{{}, rip}, code{code}, view{std::move(view)} {}

    bool IsEnd() const { return is_end; }

    bool ExecNext();
    const ThreadState& GetState() const { return state; }
};