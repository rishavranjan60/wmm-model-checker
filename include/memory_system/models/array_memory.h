#pragma once

#include "memory_system/memory.h"

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

