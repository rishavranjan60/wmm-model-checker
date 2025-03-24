#pragma once

#include <stdexcept>

class SyntaxError : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

class RuntimeError : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

class FailError : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};