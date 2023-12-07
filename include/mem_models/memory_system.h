#pragma once

#include "defs.h"
#include "path_choosers/path_chooser.h"

#include <vector>
#include <memory>
#include <iomanip>

class Memory {
public:
    virtual ~Memory() = default;

    virtual Word Load(Word address) = 0;
    virtual void Store(Word address, Word value) = 0;
    virtual size_t Size() const = 0;
    virtual void Print(std::ostream& out) const = 0;

    virtual void CheckAddress(Word address) {
        if (static_cast<size_t>(address) >= Size()) {
            throw RuntimeError{"Bad address: " + std::to_string(address)};
        }
    }
};

class MemoryView {
protected:
    std::shared_ptr<Memory> memory;
    std::shared_ptr<PathChooser> path_chooser;

public:
    MemoryView(std::shared_ptr<Memory> memory, std::shared_ptr<PathChooser> path_chooser)
        : memory(std::move(memory)), path_chooser(std::move(path_chooser)) {}

    virtual ~MemoryView() = default;
    virtual Word Load(Word address, MemoryOrder) = 0;
    virtual void Store(Word address, Word value, MemoryOrder) = 0;
    virtual void Fence(MemoryOrder) = 0;
    virtual void DoSilent() = 0;
    virtual bool HasSilent() const = 0;
    virtual void Print(std::ostream&) const = 0;
};

class ArrayMemory : public Memory {
private:
    std::vector<Word> data;

public:
    ArrayMemory(size_t size) : data(size, 0) {}

    Word Load(Word address) override {
        CheckAddress(address);
        return data[address];
    }

    void Store(Word address, Word value) override {
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
