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
        auto thread_id = std::uniform_int_distribution<size_t>{0, threads.size() - 1}(random_generator);
        for (size_t i{}; i < threads.size(); ++i) {
            if (!threads[thread_id++].IsEnd()) {
                return thread_id - 1;
            }
            thread_id %= threads.size();
        }
        throw RuntimeError{"Trying to choose thread, but all threads are finished"};
    }
};
