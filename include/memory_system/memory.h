#pragma once

#include "memory_system/memory_view.h"

#include "errors.h"

#include <vector>
#include <list>
#include <iomanip>
#include <ostream>
#include <utility>
#include <ranges>

class Memory {
public:
    virtual ~Memory() = default;

    virtual size_t Size() const = 0;
    virtual void Print(std::ostream& out) const = 0;

    virtual void SetVerbosity(bool is_verbose) = 0;

    virtual void CheckAddress(Word address) const {
        if (static_cast<size_t>(address) >= Size()) {
            throw RuntimeError{"Bad address: " + std::to_string(address)};
        }
    }
};
