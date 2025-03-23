#pragma once

#include "path_choosers/path_chooser.h"

#include "errors.h"
#include "thread.h"

class RandomChooser : public PathChooser
{
  private:
    std::mt19937_64 random_generator;

  public:
    RandomChooser(size_t seed = 239) : random_generator{seed} {}

    int ChooseThread(const std::vector<Thread> &threads,
                     const std::shared_ptr<const Memory> &) override
    {
        if (threads.empty())
        {
            throw std::logic_error{"No threads in program"};
        }
        return std::uniform_int_distribution<size_t>{0, threads.size() - 1}(random_generator);
    }

    int ChooseVariant(const std::vector<std::string> &variants, const std::string &) override
    {
        return std::uniform_int_distribution<size_t>{0, variants.size() - 1}(random_generator);
    }
};
