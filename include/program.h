#pragma once

#include "defs.h"
#include "code.h"
#include "thread.h"
#include "path_choosers/path_chooser.h"

#include <vector>

class Program {
private:
    Code code;

    std::vector<Thread> threads;
    size_t threads_count;
    std::shared_ptr<Memory> memory;
    std::shared_ptr<PathChooser> path_chooser;

    bool skip_thread_silent = true;

public:
    Program(Code code, size_t threads_count, std::shared_ptr<PathChooser> path_chooser)
        : code{std::move(code)}, threads{}, threads_count{threads_count}, memory{nullptr}, path_chooser{std::move(path_chooser)} {
        threads.reserve(threads_count);
    }

    void Init(MemoryModel, size_t memory_size = 1 << 8);
    void Run();

    void SetSkipThreadSilent(bool skip) {
        skip_thread_silent = skip;
    }
};