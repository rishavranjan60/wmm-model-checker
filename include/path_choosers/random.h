#pragma once

#include "path_choosers/path_chooser.h"

#include "thread.h"
#include "errors.h"

class RandomChooser : public PathChooser {
private:
    std::mt19937_64 random_generator;

public:
    RandomChooser(size_t seed) : random_generator{seed} {}

    int ChooseThread(const std::vector<Thread>& threads, const std::shared_ptr<const Memory> &) override {
        if (threads.empty()) {
            throw std::logic_error{"No threads in program"};
        }
        return std::uniform_int_distribution<size_t>{0, threads.size() - 1}(random_generator);
    }

    bool ExecSilent() override {
        static std::bernoulli_distribution rand_bool(0.5);
        return rand_bool(random_generator);
    }

    int ChooseSilent(const std::string&, const std::vector<std::pair<int, std::string>>& variants) override {
        auto choose = std::uniform_int_distribution<size_t>{0, variants.size() - 1}(random_generator);
        return variants[choose].first;
    }
};
