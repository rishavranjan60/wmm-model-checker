#pragma once

#include "memory_system/memory_view.h"
#include "memory_system/memory.h"

#include <deque>

class WriteBufferView : public WithMemoryAndChooserView<ArrayMemory> {
private:
    std::deque<std::pair<Word, Word>> buffer{}; // first - address; second - value;

    void Propagate() {
        auto [address, value] = buffer.back();
        memory->Store(address, value);
        buffer.pop_back();
    }

public:
    using WithMemoryAndChooserView::WithMemoryAndChooserView;

    void Store(Word address, Word value, MemoryOrder) override {
        memory->CheckAddress(address);
        buffer.emplace_front(address, value);
    }

    Word Load(Word address, MemoryOrder) override {
        for (const auto& [addr, value] : buffer) {
            if (addr == address) {
                return value;
            }
        }
        return memory->Load(address);
    }

    void Fence(MemoryOrder) override {
        while (!buffer.empty()) {
            Propagate();
        }
    }

    void DoSilent() override {
        if (!buffer.empty()) {
            Propagate();
        }
    }

    bool HasSilent() const override {
        return !buffer.empty();
    }

    void Print(std::ostream& out) const override {
        for (const auto& [address, value] : buffer) {
            out << '#' << address << " <- " << value << '\n';
        }
    }
};