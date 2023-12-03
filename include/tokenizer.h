#pragma once

#include "tokens.h"
#include "errors.h"

#include <istream>
#include <sstream>
#include <string>

class Tokenizer {
private:
    std::istream& in;
    bool is_end = false;
    tokens::Line line{};

public:
    static constexpr int kMaxTokensPerLine = 8;

    void Next();

    Tokenizer(std::istream& in) : in(in) {
        Next();
    }

    tokens::Line GetLine() const {
        return line;
    }

    bool IsEnd() const {
        return is_end;
    }
};