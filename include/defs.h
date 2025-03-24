#pragma once

#include <cstddef>
#include <cstdint>
#include <ostream>
#include <unordered_map>

enum class MemoryOrder : int8_t
{
    RLX,
    REL,
    ACQ,
    REL_ACQ,
    SEQ_CST
};
enum class BinaryOperator : int8_t
{
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    XOR
};

using Word = int32_t;
using Register = int8_t;
constexpr size_t kRegistersCount = 16;

enum class MemoryModel : int8_t
{
    SEQ_CST,
    TSO,
    PSO,
    SRA,
    RA
};

constexpr unsigned kDecimalDigitsInWord = 10;
constexpr unsigned kDecimalDigitsInRegistersCount = 2;

std::ostream &operator<<(std::ostream &, MemoryOrder);
std::ostream &operator<<(std::ostream &, BinaryOperator);
std::ostream &operator<<(std::ostream &, Register);
MemoryOrder ExtractLoadOrder(MemoryOrder);
MemoryOrder ExtractStoreOrder(MemoryOrder);
