#include "defs.h"

#include "errors.h"

std::ostream& operator<<(std::ostream& out, MemoryOrder order) {
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

std::ostream& operator<<(std::ostream& out, BinaryOperator op) {
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

std::ostream& operator<<(std::ostream& out, Register reg) {
    return out << 'r' << static_cast<int>(reg);
}

MemoryOrder ExtractLoadOrder(MemoryOrder order) {
    switch (order) {
        case MemoryOrder::RLX:
        case MemoryOrder::ACQ:
            return order;
        case MemoryOrder::REL_ACQ:
            return MemoryOrder::ACQ;
        case MemoryOrder::SEQ_CST:
        case MemoryOrder::REL:
            throw RuntimeError{"Bad memory order for reading"};
    }
}

MemoryOrder ExtractStoreOrder(MemoryOrder order) {
    switch (order) {
        case MemoryOrder::RLX:
        case MemoryOrder::REL:
            return order;
        case MemoryOrder::REL_ACQ:
            return MemoryOrder::REL;
        case MemoryOrder::SEQ_CST:
        case MemoryOrder::ACQ:
            throw RuntimeError{"Bad memory order for reading"};
    }
}
