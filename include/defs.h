#pragma once

#include <cstdint>
#include <cstddef>

enum class MemoryOrder : int8_t { RLX, REL, ACQ, REL_ACQ, SEQ_CST };
enum class BinaryOperator : int8_t { PLUS, MINUS, MULTIPLY, DIVIDE };

using Word = int32_t;
using Register = int8_t;
constexpr size_t kRegistersCount = 16;

enum class MemoryModel : int8_t { TSO, PSO };

constexpr unsigned kDecimalDigitsInWord = 10;
constexpr unsigned kDecimalDigitsInRegistersCount = 2;
