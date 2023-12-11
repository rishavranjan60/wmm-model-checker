#pragma once

#include "memory_system/memory.h"

#include <vector>
#include <random>
#include <utility>
#include <memory>

class Thread;

class PathChooser {
public:
    virtual int ChooseThread(const std::vector<Thread>&, const std::shared_ptr<const Memory>&) = 0;
    virtual int ChooseVariant(const std::vector<std::string>&, const std::string&) = 0;
    virtual ~PathChooser() = default;
};
