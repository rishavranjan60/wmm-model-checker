#pragma once

#include <cstdint>

namespace tokens {

struct Register {
    uint8_t num;  // [0; 16)
};

struct BinaryOperator {
    enum Type : int8_t { ADD, MINUS, MULTIPLY, DIVIDE } type;
};

struct AccessMode {
    enum Mode : int8_t { RLX, REL, ACQ, REL_ACQ, SEQ_CST } mode;
};

struct Assigment {};
struct ReturnAssigment {};
struct If {};
struct Goto {};
struct Load {};
struct Store {};
struct Fence {};
struct Cas {};
struct Fai {};
struct MemoryAccess {};

}  // namespace tokens