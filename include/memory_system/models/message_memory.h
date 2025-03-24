#pragma once

#include "memory_system/memory.h"

#include <optional>

class MessageMemory : public Memory
{
  public:
    struct Message;
    using Stamp = std::list<Message>::const_iterator;

    class View
    {
      private:
        std::vector<Stamp> data;

      public:
        View(size_t size = 0) : data(size) {}

        Stamp &operator[](size_t i)
        {
            return data[i];
        }
        const Stamp &operator[](size_t i) const
        {
            return data[i];
        }
        size_t Size() const
        {
            return data.size();
        }
    };

    struct Message
    {
        Word value;
        View view;
        // if true you can't insert new message before this one
        bool is_hooked = false;
    };

    size_t size;
    std::vector<std::list<Message>> data;
    View global_view;

    bool print_views = false;

  public:
    MessageMemory(size_t size) : size{size}, data{size, {{0, View{}}}}, global_view{} {}

    Stamp InsertAfter(Stamp stamp, Word address, Word value, View view, bool need_hook = false)
    {
        CheckAddress(address);
        auto it = data[address].insert(++stamp, {value, std::move(view), need_hook});
        return it->view[address] = it;
    }

    Stamp GetLastStamp(Word address) const
    {
        return --data[address].cend();
    }

    bool IsLast(Word address, const Stamp &stamp) const
    {
        return stamp == --data[address].cend();
    }

    View JoinViews(const View &lhs, const View &rhs)
    {
        if (lhs.Size() == 0)
        {
            return rhs;
        }
        if (rhs.Size() == 0)
        {
            return lhs;
        }
        if (lhs.Size() != rhs.Size())
        {
            throw std::logic_error{"Joining views with different size"};
        }
        auto max = [&](Word addr)
        {
            for (auto it = data[addr].begin(); it != data[addr].end(); ++it)
            {
                if (it == rhs[addr])
                {
                    return lhs[addr];
                }
                if (it == lhs[addr])
                {
                    return rhs[addr];
                }
            }
            throw std::logic_error{"Address not found"};
        };
        View result{lhs.Size()};
        for (Word addr{}; static_cast<size_t>(addr) < lhs.Size(); ++addr)
        {
            result[addr] = max(addr);
        }
        return result;
    }

    View UpdateGlobalView(const View &view)
    {
        return global_view = JoinViews(view, global_view);
    }

    size_t Size() const override
    {
        return size;
    }

    void PrintView(const View &view, std::ostream &out) const
    {
        auto prev_fill = out.fill('0');
        for (Word addr{}; static_cast<size_t>(addr) < view.Size(); ++addr)
        {
            out << '#' << std::setw(3) << std::right << addr << ": " << &*view[addr] << '\n';
        }
        out.fill(prev_fill);
    }

    void Print(std::ostream &out) const override
    {
        auto prev_fill = out.fill('0');
        for (Word addr{}; const auto &list : data)
        {
            out << '#' << std::setw(3) << std::right << addr++ << ": ";
            for (auto it = list.crbegin(); it != list.crend(); ++it)
            {
                out << "{" << std::setw(kDecimalDigitsInWord + 1) << it->value << " @ " << &*it
                    << "}" << (it->is_hooked ? '*' : ' ');
                if (print_views)
                {
                    out << "\nView:\n";
                    PrintView(it->view, out);
                }
            }
            out << "\n";
        }
        out.fill(prev_fill);
    }

    void SetVerbosity(bool is_verbose) override
    {
        print_views = is_verbose;
    }
};
