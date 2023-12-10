#include "program.h"

#include "mem_systems/seq_cst.h"
#include "mem_systems/tso.h"
#include "mem_systems/pso.h"

void Program::Init(MemoryModel memory_model, size_t memory_size) {
    threads.clear();
    auto default_init_threads = [&]<class View> {
        auto mem = std::make_shared<ArrayMemory>(memory_size);
        memory = mem;
        for (size_t i{}; i < threads_count; ++i) {
            threads.emplace_back(&code, std::make_unique<View>(mem, path_chooser), path_chooser, i, skip_thread_silent);
        }
    };
    switch (memory_model) {
        case MemoryModel::SEQ_CST:
            default_init_threads.operator()<DirectView>();
            break;
        case MemoryModel::TSO:
            default_init_threads.operator()<WriteBufferView>();
            break;
        case MemoryModel::PSO:
            default_init_threads.operator()<WriteBufferPerLocView>();
            break;
        default:
            throw std::logic_error{"Unimplemented memory model"};
    }
}

void Program::Run() {
    if (!memory) {
        throw RuntimeError{"Run program without init"};
    }
    while (!threads.empty()) {
        size_t thread_id = path_chooser->ChooseThread(threads, memory);
        if (threads[thread_id].ExecNext()) {
            threads.erase(threads.begin() + thread_id);
        }
    }
}