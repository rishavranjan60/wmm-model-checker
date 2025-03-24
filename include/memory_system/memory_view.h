#pragma once

#include "defs.h"
#include "path_choosers/path_chooser.h"

#include <functional>
#include <memory>

class MemoryView
{
  public:
    virtual Word Load(Word address, MemoryOrder) = 0;
    virtual void Store(Word address, Word value, MemoryOrder) = 0;
    virtual void Fence(MemoryOrder) = 0;
    virtual void DoSilent() = 0;
    virtual bool HasSilent() const = 0;
    virtual void Print(std::ostream &) const = 0;
    virtual ~MemoryView() = default;

    virtual Word Cas(Word address, Word expected, Word desired, MemoryOrder order)
    {
        auto prev = Load(address, order);
        if (prev == expected)
        {
            Store(address, desired, order);
        }
        return prev;
    }

    virtual Word Fai(Word address, Word value, MemoryOrder order)
    {
        auto prev = Load(address, order);
        Store(address, prev + value, order);
        return prev;
    }
};

template <class MemType> class WithMemoryAndChooserView : public MemoryView
{
  protected:
    std::shared_ptr<MemType> memory;
    std::shared_ptr<PathChooser> path_chooser;

  public:
    WithMemoryAndChooserView(std::shared_ptr<MemType> memory,
                             std::shared_ptr<PathChooser> path_chooser)
        : memory{std::move(memory)}, path_chooser{std::move(path_chooser)}
    {
    }
};