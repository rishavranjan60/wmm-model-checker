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

    std::string begin_str;
    std::string end_str;

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
    InteractiveChooser(std::istream& in = std::cin, std::ostream& out = std::cout, std::string begin_str = "->\n",
                       std::string end_str = "<-\n")
        : in{in}, out{out}, begin_str{std::move(begin_str)}, end_str{std::move(end_str)} {}

    void PrintHelp(std::ostream& help_out) override {
        help_out << "Write -N to view Nth thread state.\n";
        help_out << "To choose Nth thread to execute write N.\n";
        help_out << "To view memory write 0.\n";
    }

    int ChooseThread(const std::vector<Thread>& threads, const std::shared_ptr<const Memory>& memory) override {
        if (threads.empty()) {
            throw std::logic_error{"No threads in program"};
        }
        out << "Threads:\n" << begin_str;
        for (size_t i{}; const Thread& thread : threads) {
            out << ++i << ": ";
            thread.PrintCurrentCommand();
        }
        out << end_str;
        int threads_count = threads.size();
        while (true) {
            int n = GetInt(-threads_count, threads_count);
            if (n == 0) {
                out << "Memory:\n" << begin_str;
                memory->Print(out);
                out << end_str;
                continue;
            }
            bool thread_choose = n > 0;
            n = (thread_choose ? n : -n) - 1;
            if (thread_choose) {
                return n;
            }

            out << "State:\n" << begin_str;
            threads[n].GetState().Print(out);
            out << end_str;

            out << "Memory view:\n" << begin_str;
            threads[n].PrintMemView(out);
            out << end_str;
            threads[n].PrintCurrentCommand(out);
        }
    }

    int ChooseVariant(const std::vector<std::string>& variants, const std::string& hint) override {
        if (variants.empty()) {
            throw std::logic_error{"ChooseVariant with empty variants [InteractiveChooser]"};
        }
        out << "Choose: " << hint << '\n';
        for (int i{}; const auto& meaning : variants) {
            out << i++ << ": " << meaning << '\n';
        }
        return GetInt(0, variants.size() - 1);
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
    InteractiveRandomChooser(std::istream& in = std::cin, std::ostream& out = std::cout, size_t seed = 239,
                             std::string begin_str = "->\n", std::string end_str = "<-\n")
        : InteractiveChooser(in, out, std::move(begin_str), std::move(end_str)), random_generator{seed} {}

    int ChooseThread(const std::vector<Thread>& threads, const std::shared_ptr<const Memory>& memory) override {
        if (threads.empty()) {
            throw std::logic_error{"No threads in program"};
        }
        auto n = std::uniform_int_distribution<size_t>{0, threads.size() - 1}(random_generator);

        out << "Memory:\n" << begin_str;
        memory->Print(out);
        out << end_str;

        out << "State:\n" << begin_str;
        threads[n].GetState().Print(out);
        out << end_str;

        out << "Memory view:\n" << begin_str;
        threads[n].PrintMemView(out);
        out << end_str;

        threads[n].PrintCurrentCommand(out);

        out << "> " << n + 1 << '\n';
        return n;
    }
};