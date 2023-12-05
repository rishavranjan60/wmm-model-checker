#include "program.h"

void Program::Init(MemoryModel memory_model) {
    if (memory_model == MemoryModel::TSO) {
        threads.resize(threads_count, {&code});
    } else {
        throw std::logic_error{"Unimplemented memory model"};
    }
    alive_threads = threads.size();
}

void Program::Run() {
    if (alive_threads == -1ull) {
        throw RuntimeError{"Run program without init"};
    }
    while (alive_threads > 0) {
        size_t thread_id = path_chooser->ChooseThread(threads);
        alive_threads -= threads[thread_id].ExecNext();
    }
    alive_threads = -1;
}