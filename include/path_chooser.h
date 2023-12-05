#pragma once

#include "thread.h"

#include <vector>
#include <random>

class PathChooser {
public:
    virtual size_t ChooseThread(const std::vector<Thread>&) = 0;
    virtual ~PathChooser() = default;
};

class RandomChooser : public PathChooser {
private:
    std::mt19937_64 random_generator;

public:
    RandomChooser(size_t seed) : random_generator{seed} {}

    size_t ChooseThread(const std::vector<Thread>& threads) override {
        return std::uniform_int_distribution<size_t>{0, threads.size() - 1}(random_generator);
    }
};