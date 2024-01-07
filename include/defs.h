#pragma once

#include <cstdint>
#include <cstddef>
#include <ostream>
#include <unordered_map>

enum class MemoryOrder : int8_t { RLX, REL, ACQ, REL_ACQ, SEQ_CST };
enum class BinaryOperator : int8_t { PLUS, MINUS, MULTIPLY, DIVIDE, XOR };

using Word = int32_t;
using Register = int8_t;
constexpr size_t kRegistersCount = 16;

enum class MemoryModel : int8_t { SEQ_CST, TSO, PSO, SRA, RA };

constexpr unsigned kDecimalDigitsInWord = 10;
constexpr unsigned kDecimalDigitsInRegistersCount = 2;

inline std::ostream& operator<<(std::ostream& out, MemoryOrder order) {
    switch (order) {
        case MemoryOrder::RLX:
            out << "RLX";
            break;
        case MemoryOrder::REL:
            out << "REL";
            break;
        case MemoryOrder::ACQ:
            out << "ACQ";
            break;
        case MemoryOrder::REL_ACQ:
            out << "REL_ACQ";
            break;
        case MemoryOrder::SEQ_CST:
            out << "SEQ_CST";
            break;
    }
    return out;
}

inline std::ostream& operator<<(std::ostream& out, BinaryOperator op) {
    switch (op) {
        case BinaryOperator::PLUS:
            out << '+';
            break;
        case BinaryOperator::MINUS:
            out << '-';
            break;
        case BinaryOperator::MULTIPLY:
            out << '*';
            break;
        case BinaryOperator::DIVIDE:
            out << '/';
            break;
        case BinaryOperator::XOR:
            out << '^';
            break;
    }
    return out;
}

inline std::ostream& operator<<(std::ostream& out, Register reg) {
    return out << 'r' << static_cast<int>(reg);
}
