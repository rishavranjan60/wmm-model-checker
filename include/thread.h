#pragma once

#include "defs.h"
#include "errors.h"

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
};

std::ostream& operator<<(std::ostream&, const ThreadState&);

class Thread {
private:
    ThreadState state;
    const Code* code;
    bool is_end = false;

public:
    Thread(const Code* code, Word rip = 0) : state{{}, rip}, code{code} {}

    bool IsEnd() const { return is_end; }

    bool ExecNext();
    const ThreadState& GetState() const { return state; }
};