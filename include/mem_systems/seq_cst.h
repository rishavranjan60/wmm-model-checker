#pragma once

#include "mem_systems/memory_view.h"
#include "mem_systems/memory.h"

#include <memory>

class DirectView : public WithMemoryAndChooserView<ArrayMemory> {
public:
    using WithMemoryAndChooserView::WithMemoryAndChooserView;

    Word Load(Word address, MemoryOrder) override { return memory->Load(address); }

    void Store(Word address, Word value, MemoryOrder) override { return memory->Store(address, value); }

    void Fence(MemoryOrder) override {}

    void DoSilent() override {
        throw std::logic_error{"Silent step in DirectView"};
    }

    bool HasSilent() const override {
        return false;
    }

    void Print(std::ostream& out) const override {
        out << "Same as memory\n";
    }
};