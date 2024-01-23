#pragma once

#include "memory_system/memory_view.h"
#include "memory_system/models/message_memory.h"

#include <memory>
#include <optional>

enum class RAWeakness { Weak, Strong };

template <RAWeakness weakness>
class TimestampView : public WithMemoryAndChooserView<MessageMemory> {
protected:
    std::vector<MessageMemory::View> rel;
    MessageMemory::View cur;
    MessageMemory::View acq;

    void ReleaseFence() {
        if (rel.size() != cur.Size()) {
            throw std::logic_error{"rel size != memory size"};
        }
        for (Word addr{}; static_cast<size_t>(addr) < cur.Size(); ++addr) {
            rel[addr] = cur;
        }
    }

public:
    TimestampView(std::shared_ptr<MessageMemory> memory, std::shared_ptr<PathChooser> path_chooser)
        : WithMemoryAndChooserView(std::move(memory), std::move(path_chooser)), rel(this->memory->Size()), cur(this->memory->Size()) {
        for (Word addr{}; static_cast<size_t>(addr) < cur.Size(); ++addr) {
            cur[addr] = this->memory->GetLastStamp(addr);
        }
        acq = cur;
        ReleaseFence();
    }

    Word Load(Word address, MemoryOrder order) override {
        while (!memory->IsLast(address, cur[address])) {
            if (!path_chooser->ChooseVariant({"skip", "increase"},
                                             "increase timestamp of #" + std::to_string(address))) {
                break;
            }
            ++cur[address];
            acq = memory->JoinViews(acq, cur[address]->second);
            switch (order) {
                case MemoryOrder::RLX:
                case MemoryOrder::REL:
                    break;
                case MemoryOrder::ACQ:
                case MemoryOrder::REL_ACQ:
                    cur = acq;
                    break;
                case MemoryOrder::SEQ_CST:
                    throw std::logic_error{"SEQ_CST accesses unimplemented for ra/sra"};
                default:
                    throw std::logic_error{"Unimplemented memory model"};
            }
        }
        return cur[address]->first;
    }

    void Store(Word address, Word value, MemoryOrder order) override {
        auto& stamp = cur[address];
        while (!memory->IsLast(address, stamp)) {
            if constexpr (weakness == RAWeakness::Weak) {
                if (!path_chooser->ChooseVariant({"this", "next"}, "insert after")) {
                    break;
                }
            }
            ++stamp;
            acq = memory->JoinViews(acq, stamp->second);
        }
        switch (order) {
            case MemoryOrder::RLX:
            case MemoryOrder::ACQ:
                break;
            case MemoryOrder::REL:
            case MemoryOrder::REL_ACQ:
                rel[address] = cur;
                break;
            case MemoryOrder::SEQ_CST:
                throw std::logic_error{"SEQ_CST accesses unimplemented for ra/sra"};
            default:
                throw std::logic_error{"Unimplemented memory model"};
        }
        stamp = memory->InsertAfter(stamp, address, value, rel[address]);
        acq = memory->JoinViews(acq, cur);
        rel[address][address] = stamp;
    }

    Word Fai(Word address, Word value, MemoryOrder order) override {
        return MemoryView::Fai(address, value, order);  // TODO
    }

    Word Cas(Word address, Word expected, Word desired, MemoryOrder order) override {
        return MemoryView::Cas(address, expected, desired, order);  // TODO
    }

    void Fence(MemoryOrder order) override {
        switch (order) {
            case MemoryOrder::RLX:
                throw RuntimeError{"RLX fence doesn't make sense"};
            case MemoryOrder::REL:
                ReleaseFence();
                break;
            case MemoryOrder::ACQ:
                cur = acq;
                break;
            case MemoryOrder::REL_ACQ:
                cur = acq;
                ReleaseFence();
                break;
            case MemoryOrder::SEQ_CST:
                cur = acq;
                ReleaseFence();
                // TODO
                break;
            default:
                throw std::logic_error{"Unimplemented memory order fence [Release/Acquire]"};
        }
    }

    void DoSilent() override { throw std::logic_error{"DoSilent in SRA"}; }

    bool HasSilent() const override { return false; }

    void Print(std::ostream& out) const override { memory->PrintView(cur, out); }
};