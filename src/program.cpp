#include "program.h"

#include "memory_system/models/seq_cst.h"
#include "memory_system/models/tso.h"
#include "memory_system/models/pso.h"
#include "memory_system/models/sra.h"

void Program::Init(MemoryModel memory_model, size_t memory_size) {
    threads.clear();
    auto default_init_threads = [&]<class MemType, class View> {
        auto mem = std::make_shared<MemType>(memory_size);
        memory = mem;
        for (size_t i{}; i < threads_count; ++i) {
            threads.emplace_back(&code, std::make_unique<View>(mem, path_chooser), path_chooser, i, skip_thread_silent);
        }
    };
    switch (memory_model) {
        case MemoryModel::SEQ_CST:
            default_init_threads.operator()<ArrayMemory, DirectView>();
            break;
        case MemoryModel::TSO:
            default_init_threads.operator()<ArrayMemory, WriteBufferView>();
            break;
        case MemoryModel::PSO:
            default_init_threads.operator()<ArrayMemory, WriteBufferPerLocView>();
            break;
        case MemoryModel::SRA:
            default_init_threads.operator()<MessageMemory, GlobalTimestampView>();
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