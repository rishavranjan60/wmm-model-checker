#pragma once

#include "defs.h"

#include <vector>

class Memory {
public:
    virtual ~Memory() = default;

    virtual Word Load(Word address) = 0;
    virtual void Store(Word address, Word value) = 0;
    virtual size_t Size() const = 0;
};

class MemoryView {
public:
    virtual ~MemoryView() = default;
    virtual Word Load(Word address, MemoryOrder) = 0;
    virtual Word Store(Word address, Word value, MemoryOrder) = 0;
    virtual void Fence(MemoryOrder) = 0;
};

class ArrayMemory : public Memory {
private:
    std::vector<Word> data;
public:
    ArrayMemory(size_t size) : data(size, 0) {}

    Word Load(Word address) override {
        return data[address];
    }

    void Store(Word address, Word value) override {
        data[address] = value;
    }

    size_t Size() const override {
        return data.size();
    }
};