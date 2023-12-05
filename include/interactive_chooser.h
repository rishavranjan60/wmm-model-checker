#pragma once

#include "path_chooser.h"

#include <iostream>

class InteractiveChooser : public PathChooser {
private:
    std::istream& in;
    std::ostream& out;

public:
    InteractiveChooser(std::istream& in = std::cin, std::ostream& out = std::cout) : in(in), out(out) {
        out << "Write -N to view Nth thread state. To choose Nth thread to execute write N\n";
    }

    size_t ChooseThread(const std::vector<Thread>& threads) override {
        int n;
        while (in >> n) {
            if (n == 0) {
                out << "Write non zero integer for execute or view thread\n";
                continue;
            }
            bool thread_choose = n > 0;
            n = (thread_choose ? n : -n) - 1;
            if (n >= threads.size()) {
                out << "Thread number out of bounds, threads count: " << threads.size() << '\n';
                continue;
            }
            if (thread_choose) {
                return n;
            }
            out << threads[n].GetState();
        }
        throw RuntimeError{"Input is not integer or ended unexpectedly"};
    }
};