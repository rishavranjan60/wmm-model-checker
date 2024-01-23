#pragma once

#include "memory_system/memory.h"

#include <optional>

class MessageMemory : public Memory {
public:
    class View;
    using Stamp = std::list<std::pair<Word, View>>::const_iterator;

    class View {
    private:
        std::vector<Stamp> data;

    public:
        View(size_t size = 0) : data(size) {}

        Stamp& operator[](size_t i) { return data[i]; }
        const Stamp& operator[](size_t i) const { return data[i]; }
        size_t Size() const { return data.size(); }
    };

    size_t size;
    std::vector<std::list<std::pair<Word, View>>> data;

    bool print_views = false;

public:
    MessageMemory(size_t size) : size{size}, data{size, {{0, View{}}}} {}

    Stamp InsertAfter(Stamp stamp, Word address, Word value, View view) {
        CheckAddress(address);
        auto it = data[address].emplace(++stamp, value, std::move(view));
        return it->second[address] = it;
    }

    Stamp GetLastStamp(Word address) const { return --data[address].cend(); }

    bool IsLast(Word address, const Stamp& stamp) const { return stamp == --data[address].cend(); }

    View JoinViews(const View& lhs, const View& rhs) {
        if (lhs.Size() == 0) {
            return rhs;
        }
        if (rhs.Size() == 0) {
            return lhs;
        }
        if (lhs.Size() != rhs.Size()) {
            throw std::logic_error{"Joining views with different size"};
        }
        auto max = [&](Word addr) {
            for (auto it = data[addr].begin(); it != data[addr].end(); ++it) {
                if (it == rhs[addr]) {
                    return lhs[addr];
                }
                if (it == lhs[addr]) {
                    return rhs[addr];
                }
            }
            throw std::logic_error{"Address not found"};
        };
        View result{lhs.Size()};
        for (Word addr{}; static_cast<size_t>(addr) < lhs.Size(); ++addr) {
            result[addr] = max(addr);
        }
        return result;
    }

    size_t Size() const override { return size; }

    void PrintView(const View& view, std::ostream& out) const {
        auto prev_fill = out.fill('0');
        for (Word addr{}; static_cast<size_t>(addr) < view.Size(); ++addr) {
            out << '#' << std::setw(3) << std::right << addr << ": " << &*view[addr] << '\n';
        }
        out.fill(prev_fill);
    }

    void Print(std::ostream& out) const override {
        auto prev_fill = out.fill('0');
        for (Word addr{}; const auto& list : data) {
            out << '#' << std::setw(3) << std::right << addr++ << ":";
            for (auto it = list.crbegin(); it != list.crend(); ++it) {
                const auto& [value, view] = *it;
                out << " {" << std::setw(kDecimalDigitsInWord + 1) << value << " @ " << &*it << "}";
                if (print_views) {
                    out << "\nView:\n";
                    PrintView(view, out);
                }
            }
            out << "\n";
        }
        out.fill(prev_fill);
    }

    void SetVerbosity(bool is_verbose) override { print_views = is_verbose; }
};
