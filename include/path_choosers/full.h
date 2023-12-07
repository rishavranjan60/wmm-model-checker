#pragma once

#include "path_choosers/path_chooser.h"

#include <vector>

class FullChooser : public PathChooser {
private:
    struct State {
        int min;
        int max;
        int cur = min;
    };

    std::vector<State> path{};
    size_t cur = 0;

    bool is_end = false;

    int Go(int min, int max) {
        if (cur == path.size()) {
            path.emplace_back(min, max);
        }
        if (min != path[cur].min || max != path[cur].max) {
            throw std::logic_error{"Go params don't match"};
        }
        return path[cur++].cur;
    }

    void Next() {
        if (cur-- != path.size()) {
            throw std::logic_error{"NextRun not at leaf"};
        }
        while (path[cur].cur++ == path[cur].max) {
            path.pop_back();
            if (cur-- == 0) {
                is_end = true;
                break;
            }
        }
    }

public:
    int ChooseThread(const std::vector<Thread>& threads, const std::shared_ptr<const Memory>& memory) override {
        if (threads.empty()) {
            throw std::logic_error{"No threads in program"};
        }
        return Go(1, threads.size()) - 1;
    }

    int ChooseSilent(const std::string& hint, const std::vector<std::string>& variants) override {
        if (variants.empty()) {
            throw std::logic_error{"ChooseSilent with empty variants [InteractiveChooser]"};
        }
        return Go(0, variants.size() - 1);
    }

    bool ExecSilent() override { return Go(0, 1); }

    bool Finished() const { return is_end; }

    void NextRun() {
        Next();
        cur = 0;
    }

    std::vector<int> GetTrace() const {
        std::vector<int> res;
        res.reserve(path.size());
        for (const auto& [min, max, c] : path) {
            res.push_back(c);
        }
        return res;
    }
};