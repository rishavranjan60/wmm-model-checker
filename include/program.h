#pragma once

#include "defs.h"
#include "code.h"
#include "thread.h"
#include "path_chooser.h"

#include <vector>

class Program {
private:
    Code code;

    std::vector<Thread> threads;
    size_t threads_count;
    std::unique_ptr<PathChooser> path_chooser;

public:
    Program(Code code, size_t threads_count, std::unique_ptr<PathChooser> path_chooser)
        : code{std::move(code)}, threads{}, threads_count{threads_count}, path_chooser{std::move(path_chooser)} {
        threads.reserve(threads_count);
    }

    void Init(MemoryModel);
    void Run();
};