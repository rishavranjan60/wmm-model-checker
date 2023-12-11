#pragma once

#include "defs.h"
#include "path_choosers/path_chooser.h"

#include <memory>

class MemoryView {
public:
    virtual Word Load(Word address, MemoryOrder) = 0;
    virtual void Store(Word address, Word value, MemoryOrder) = 0;
    virtual void Fence(MemoryOrder) = 0;
    virtual void DoSilent() = 0;
    virtual bool HasSilent() const = 0;
    virtual void Print(std::ostream&) const = 0;
    virtual ~MemoryView() = default;
};

template <class MemType>
class WithMemoryAndChooserView : public MemoryView {
protected:
    std::shared_ptr<MemType> memory;
    std::shared_ptr<PathChooser> path_chooser;

public:
    WithMemoryAndChooserView(std::shared_ptr<MemType> memory, std::shared_ptr<PathChooser> path_chooser)
        : memory{std::move(memory)}, path_chooser{std::move(path_chooser)} {}
};