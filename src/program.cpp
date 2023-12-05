#include "program.h"

void Program::Init(MemoryModel memory_model) {
    if (memory_model == MemoryModel::TSO) {
    } else {
        throw std::logic_error{"Unimplemented memory model"};
    }
}

void Program::Run() {
    // TODO
}