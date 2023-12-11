#pragma once

#include "memory_system/memory.h"

#include <optional>

class MessageMemory : public Memory {
private:
    struct ViewWrapper;
    using Stamp = std::list<std::pair<Word, ViewWrapper>>::const_iterator;

public:
    using View = std::vector<Stamp>;

private:
    struct ViewWrapper {
        View view;
    };
    size_t size;
    std::vector<std::list<std::pair<Word, ViewWrapper>>> data;

    bool print_views = false;

    View init_view;

public:
    MessageMemory(size_t size) : size{size}, data{size}, init_view{} {
        init_view.reserve(size);
        for (auto& list : data) {
            list.emplace_back(0, ViewWrapper{View{size}});
            init_view.emplace_back(list.cbegin());
        }
        for (auto& list : data) {
            list.front().second.view = init_view;
        }
    }

    View InsertAfterLast(Word address, Word value, std::optional<View> view) {
        data[address].emplace_back(value, view ? std::move(*view) : init_view);
        auto& new_view = data[address].back().second.view;
        new_view[address] = --data[address].cend();
        return new_view;
    }

    Stamp GetLastStamp(Word address) const { return --data[address].cend(); }

    bool IsLast(Word address, const Stamp& stamp) const { return stamp == --data[address].cend(); }

    View JoinViews(View lhs, const View& rhs) {
        if (lhs.size() != rhs.size()) {
            throw std::logic_error{"Joining views with different size"};
        }
        auto min = [&](Word addr) {
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
        for (Word addr{}; static_cast<size_t>(addr) < rhs.size(); ++addr) {
            lhs[addr] = min(addr);
        }
        return lhs;
    }

    size_t Size() const override { return size; }

    void PrintView(const View& view, std::ostream& out) const {
        auto prev_fill = out.fill('0');
        for (Word addr{}; const auto& stamp : view) {
            out << '#' << std::setw(3) << std::right << addr++ << ": " << &*stamp << '\n';
        }
        out.fill(prev_fill);
    }

    void Print(std::ostream& out) const override {
        auto prev_fill = out.fill('0');
        for (Word addr{}; const auto& list : data) {
            out << '#' << std::setw(3) << std::right << addr++ << ":";
            for (auto it = list.crbegin(); it != list.crend(); ++it) {
                const auto& [value, wrapper] = *it;
                out << " {" << std::setw(kDecimalDigitsInWord + 1) << value << " @ " << &*it << "}";
                if (print_views) {
                    out << "\nView:\n";
                    PrintView(wrapper.view, out);
                }
            }
            out << "\n";
        }
        out.fill(prev_fill);
    }

    void PrintViews(bool print = true) { print_views = print; }
};
