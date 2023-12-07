#pragma once

#include "defs.h"

#include "errors.h"

#include <cstdint>
#include <variant>
#include <vector>
#include <optional>
#include <string>

#define DEFINE_EQUALS(Self) bool operator==(const Self&) const = default

#define DEFINE_TOKEN(Name)                          \
    struct Name {                                   \
        DEFINE_EQUALS(Name);                        \
        static constexpr char kTokenName[] = #Name; \
    };

#define DEFINE_TOKEN_WITH_ARG(Name, ArgT, arg_name)           \
    struct Name {                                             \
        ArgT arg_name;                                        \
        Name(ArgT arg_name) : arg_name(std::move(arg_name)) {} \
        DEFINE_EQUALS(Name);                                  \
        static constexpr char kTokenName[] = #Name;           \
    };

namespace tokens {

DEFINE_TOKEN_WITH_ARG(Register, ::Register, reg)
DEFINE_TOKEN_WITH_ARG(BinaryOperator, ::BinaryOperator, op)
DEFINE_TOKEN_WITH_ARG(MemoryOrder, ::MemoryOrder, order)
DEFINE_TOKEN_WITH_ARG(Constant, Word, value)
DEFINE_TOKEN_WITH_ARG(Label, std::string, name)
DEFINE_TOKEN_WITH_ARG(Goto, Label, label)
DEFINE_TOKEN_WITH_ARG(MemoryAt, Register, reg)

DEFINE_TOKEN(Assigment)
DEFINE_TOKEN(ReturnAssigment)
DEFINE_TOKEN(If)
DEFINE_TOKEN(Load)
DEFINE_TOKEN(Store)
DEFINE_TOKEN(Fence)
DEFINE_TOKEN(Cas)
DEFINE_TOKEN(Fai)
DEFINE_TOKEN(Finish)
DEFINE_TOKEN(Fail)

using Token = std::variant<Register, BinaryOperator, MemoryOrder, Constant, Label, Goto, MemoryAt, Assigment,
                           ReturnAssigment, If, Load, Store, Fence, Cas, Fai, Finish, Fail>;
using Line = std::vector<Token>;

template <typename T>
bool Is(const Token& token) {
    return std::get_if<T>(&token);
}

template <typename T>
T AsThrows(Token token) {
    if (!Is<T>(token)) {
        throw SyntaxError{std::string{"Expected \""} + T::kTokenName + "\" token"};
    }
    return std::get<T>(token);
}

template <typename T>
std::optional<T> As(Token token) {
    if (!Is<T>(token)) {
        return {};
    }
    return std::get<T>(token);
}

}  // namespace tokens

#undef DEFINE_EQUALS
#undef DEFINE_TOKEN
#undef DEFINE_TOKEN_WITH_ARG
