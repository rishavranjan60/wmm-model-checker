#pragma once

#include "mem_systems/memory_view.h"

#include "errors.h"

#include <vector>
#include <iomanip>
#include <ostream>

class Memory {
public:
    virtual ~Memory() = default;

    virtual size_t Size() const = 0;
    virtual void Print(std::ostream& out) const = 0;

    virtual void CheckAddress(Word address) const {
        if (static_cast<size_t>(address) >= Size()) {
            throw RuntimeError{"Bad address: " + std::to_string(address)};
        }
    }
};

class ArrayMemory : public Memory {
private:
    std::vector<Word> data;

public:
    ArrayMemory(size_t size) : data(size, 0) {}

    Word Load(Word address) const {
        CheckAddress(address);
        return data[address];
    }

    void Store(Word address, Word value) {
        CheckAddress(address);
        data[address] = value;
    }

    size_t Size() const override { return data.size(); }

    void Print(std::ostream& out) const override {
        constexpr size_t kWordsInOneRow = 8;
        for (size_t i{}; const Word& value : data) {
            if (i++ % kWordsInOneRow == 0) {
                out << std::setw(4) << std::right << i - 1 << ": ";
            }
            out << std::setw(kDecimalDigitsInWord + 1) << std::left << value << (i % kWordsInOneRow == 0 ? '\n' : ' ');
        }
    }
};
