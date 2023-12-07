#pragma once

#include "path_choosers/path_chooser.h"

#include "thread.h"
#include "errors.h"

#include <iostream>

class InteractiveChooser : public PathChooser {
private:
    std::istream& in;
    std::ostream& out;

public:
    InteractiveChooser(std::istream& in = std::cin, std::ostream& out = std::cout) : in(in), out(out) {
        out << "Write -N to view Nth thread state.\n";
        out << "To choose Nth thread to execute write N.\n";
        out << "To view memory write 0.\n";
    }

    size_t ChooseThread(const std::vector<Thread>& threads, const std::shared_ptr<const Memory>& memory) override {
        if (threads.empty()) {
            throw std::logic_error{"No threads in program"};
        }
        int n;
        while (in >> n) {
            if (n == 0) {
                memory->Print(out);
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
            out << "State:\n";
            threads[n].GetState().Print(out);
            out << "Memory view:\n";
            threads[n].PrintMemView(out);
        }
        throw RuntimeError{"Input is not integer or ended unexpectedly"};
    }

    int ChooseSilent(const std::string& hint, const std::vector<std::pair<int, std::string>>& variants) override {
        out << "Choose: " << hint << '\n';
        for (const auto& [value, meaning] : variants) {
            out << value << ": " << meaning << '\n';
        }
        int res;
        while (in >> res) {
            if (std::find_if(variants.begin(), variants.end(), [&](const auto& item) { return item.first == res; }) !=
                variants.end()) {
                return res;
            }
            out << "No value \"" << res << "\" in list, try again\n";
        }
        throw RuntimeError{"Can't read your choose"};
    }

    bool ExecSilent() override {
        out << "0 - Execute silent step\n";
        out << "1 - Execute non-silent step\n";
        int res;
        while (in >> res) {
            if (res == 0 || res == 1) {
                return !res;
            }
            out << "Write only 0 or 1\n";
        }
        throw RuntimeError{"Can't read your choose"};
    }
};