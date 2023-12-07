#include "program.h"

#include "mem_models/seq_cst.h"
#include "mem_models/tso.h"
#include "mem_models/pso.h"

void Program::Init(MemoryModel memory_model, size_t memory_size) {
    auto default_init_threads = [&]<class View> {
        memory = std::make_shared<ArrayMemory>(memory_size);
        for (size_t i{}; i < threads_count; ++i) {
            threads.emplace_back(&code, std::make_unique<View>(memory, path_chooser), path_chooser, i);
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
    alive_threads = threads.size();
}

void Program::Run() {
    if (!memory) {
        throw RuntimeError{"Run program without init"};
    }
    while (alive_threads > 0) {
        size_t thread_id = path_chooser->ChooseThread(threads, memory);
        alive_threads -= threads[thread_id].ExecNext();
    }
}