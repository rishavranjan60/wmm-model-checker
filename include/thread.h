#pragma once

#include "defs.h"

#include <array>
#include <cstdint>
#include <cstddef>

struct ThreadState {
    // r15 - store result of last arithmetic operation or last constant
    std::array<Word, kRegistersCount> registers;
    size_t rip;
};

class Thread {
private:
    ThreadState state;
    bool is_end = false;

public:

    bool IsEnd() const {
        return is_end;
    }
};