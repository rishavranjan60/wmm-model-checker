#pragma once

#include "path_choosers/path_chooser.h"

#include "thread.h"
#include "errors.h"

#include <iostream>
#include <random>

class InteractiveChooser : public PathChooser {
protected:
    std::istream& in;
    std::ostream& out;

    virtual int GetInt(int min, int max) {
        int inp;
        while (true) {
            out << "> ";
            if (in >> inp) {
                if (min <= inp && inp <= max) {
                    return inp;
                }
                out << "You should write integer in [" << min << "; " << max << "]\n";
            } else {
                throw RuntimeError{"Can't read your choose"};
            }
        }
    }

public:
    InteractiveChooser(std::istream& in = std::cin, std::ostream& out = std::cout) : in(in), out(out) {
        out << "Write -N to view Nth thread state.\n";
        out << "To choose Nth thread to execute write N.\n";
        out << "To view memory write 0.\n";
    }

    int ChooseThread(const std::vector<Thread>& threads, const std::shared_ptr<const Memory>& memory) override {
        if (threads.empty()) {
            throw std::logic_error{"No threads in program"};
        }
        int threads_count = threads.size();
        while (true) {
            int n = GetInt(-threads_count, threads_count);
            if (n == 0) {
                memory->Print(out);
                continue;
            }
            bool thread_choose = n > 0;
            n = (thread_choose ? n : -n) - 1;
            if (thread_choose) {
                return n;
            }
            out << "State:\n";
            threads[n].GetState().Print(out);
            out << "Memory view:\n";
            threads[n].PrintMemView(out);
        }
    }

    int ChooseSilent(const std::string& hint, const std::vector<std::string>& variants) override {
        if (variants.empty()) {
            throw std::logic_error{"ChooseSilent with empty variants [InteractiveChooser]"};
        }
        out << "Choose: " << hint << '\n';
        for (int i{}; const auto& meaning : variants) {
            out << i++ << ": " << meaning << '\n';
        }
        return GetInt(0, variants.size() - 1);
    }

    bool ExecSilent() override {
        out << "0 - Execute non-silent step\n";
        out << "1 - Execute silent step\n";
        return GetInt(0, 1);
    }
};

class InteractiveRandomChooser : public InteractiveChooser {
private:
    std::mt19937_64 random_generator;

protected:
    int GetInt(int min, int max) override {
        int res = std::uniform_int_distribution<int>{min, max}(random_generator);
        out << "> " << res << '\n';
        return res;
    }

public:
    InteractiveRandomChooser(std::istream& in = std::cin, std::ostream& out = std::cout, size_t seed = 239)
        : InteractiveChooser(in, out), random_generator{seed} {}

    int ChooseThread(const std::vector<Thread>& threads, const std::shared_ptr<const Memory>& memory) override {
        if (threads.empty()) {
            throw std::logic_error{"No threads in program"};
        }
        auto n = std::uniform_int_distribution<size_t>{0, threads.size() - 1}(random_generator);
        out << "Memory:\n";
        memory->Print(out);
        out << "State:\n";
        threads[n].GetState().Print(out);
        out << "Memory view:\n";
        threads[n].PrintMemView(out);
        out << "> " << n + 1 << '\n';
        return n;
    }
};