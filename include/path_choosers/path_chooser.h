#pragma once

#include "memory_system/memory.h"

#include <iostream>
#include <memory>
#include <random>
#include <utility>
#include <vector>

class Thread;

class PathChooser
{
  public:
    virtual int ChooseThread(const std::vector<Thread> &,
                             const std::shared_ptr<const Memory> &) = 0;
    virtual int ChooseVariant(const std::vector<std::string> &, const std::string &) = 0;
    virtual void PrintHelp(std::ostream & = std::cout) {}
    virtual ~PathChooser() = default;
};
