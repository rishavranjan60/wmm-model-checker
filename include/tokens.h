#pragma once

#include "defs.h"

#include <cstdint>
#include <variant>
#include <vector>
#include <string>

#define DEFINE_EQUALS(Self) bool operator==(const Self&) const = default

#define DEFINE_TOKEN(Name)   \
    struct Name {            \
        DEFINE_EQUALS(Name); \
    };

namespace tokens {

struct Register {
    ::Register reg;
    Register(::Register reg) : reg(reg) {}
    DEFINE_EQUALS(Register);
};

struct BinaryOperator {
    ::BinaryOperator op;
    BinaryOperator(::BinaryOperator op) : op(std::move(op)) {}
    DEFINE_EQUALS(BinaryOperator);
};

struct MemoryModel {
    ::MemoryModel model;
    MemoryModel(::MemoryModel model) : model(std::move(model)) {}
    DEFINE_EQUALS(MemoryModel);
};

struct Constant {
    int32_t value;
    Constant(int32_t value) : value(value) {}
    DEFINE_EQUALS(Constant);
};

struct Label {
    std::string name;
    Label(std::string name) : name(std::move(name)) {}
    DEFINE_EQUALS(Label);
};

struct Goto {
    Label label;
    Goto(Label label) : label(std::move(label)) {}
    DEFINE_EQUALS(Goto);
};

struct MemoryAt {
    Register r;
    MemoryAt(Register r) : r(std::move(r)) {}
    DEFINE_EQUALS(MemoryAt);
};

DEFINE_TOKEN(Assigment)
DEFINE_TOKEN(ReturnAssigment)
DEFINE_TOKEN(If)
DEFINE_TOKEN(Load)
DEFINE_TOKEN(Store)
DEFINE_TOKEN(Fence)
DEFINE_TOKEN(Cas)
DEFINE_TOKEN(Fai)

using Token = std::variant<Register, BinaryOperator, MemoryModel, Constant, Label, Goto, MemoryAt, Assigment, ReturnAssigment, If,
                           Load, Store, Fence, Cas, Fai>;
using Line = std::vector<Token>;

template<typename T>
bool Is(const Token& token) {
    return std::get_if<T>(&token);
}

}  // namespace tokens

#undef DEFINE_EQUALS
#undef DEFINE_TOKEN
