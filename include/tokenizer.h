#pragma once

#include <istream>
#include "tokens.h"

class Tokenizer {
private:
    std::istream& in;

public:
    Tokenizer(std::istream& in) : in(in) {
    }
};