#pragma once

#include "defs.h"
#include "errors.h"
#include "memory_system/memory_view.h"
#include "path_choosers/path_chooser.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>
#include <vector>

class Command;
using Code = std::vector<std::unique_ptr<Command>>;

struct ThreadState
{
    // r15 - store result of last arithmetic operation or last constant
    std::array<Word, kRegistersCount> registers;
    Word rip;

    Word operator[](Register reg) const
    {
        CheckReg(reg);
        return registers[reg];
    }

    Word &operator[](Register reg)
    {
        CheckReg(reg);
        return registers[reg];
    }

    Word &AuxReg()
    {
        return registers.back();
    }

    void Print(std::ostream &) const;

  private:
    void CheckReg(Register reg) const
    {
        if (static_cast<size_t>(reg) >= kRegistersCount)
        {
            throw std::logic_error{"ThreadState: register out of bounds"};
        }
    }
};

class Thread
{
  private:
    ThreadState state;
    const Code *code;
    bool is_end = false;

    std::unique_ptr<MemoryView> view;
    std::shared_ptr<PathChooser> path_chooser;

    bool skip_silent;

    bool Execute();

  public:
    Thread(const Code *code, std::unique_ptr<MemoryView> view,
           std::shared_ptr<PathChooser> path_chooser, size_t thread_id, bool skip_silent = true)
        : state{{}, 0}, code{code}, view{std::move(view)}, path_chooser{std::move(path_chooser)},
          skip_silent{skip_silent}
    {
        state.AuxReg() = thread_id;
        if (skip_silent)
        {
            Execute();
        }
    }

    bool IsEnd() const
    {
        return is_end;
    }

    bool ExecNext();
    void PrintCurrentCommand(std::ostream &out = std::cout) const;
    const ThreadState &GetState() const
    {
        return state;
    }
    void PrintMemView(std::ostream &out = std::cout) const
    {
        view->Print(out);
    }
};