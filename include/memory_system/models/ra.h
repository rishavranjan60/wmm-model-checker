#pragma once

#include "memory_system/memory_view.h"
#include "memory_system/models/message_memory.h"

#include <memory>
#include <optional>

enum class RAWeakness { Weak, Strong };

template <RAWeakness weakness>
class TimestampView : public WithMemoryAndChooserView<MessageMemory> {
private:
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

    void Store(Word address, Word value, MemoryOrder order, bool is_update) {
        auto& stamp = cur[address];
        while (!memory->IsLast(address, stamp)) {
            if constexpr (weakness == RAWeakness::Weak) {
                if (!std::next(stamp)->is_hooked && !path_chooser->ChooseVariant({"this", "next"}, "insert after")) {
                    break;
                }
            }
            ++stamp;
            acq = memory->JoinViews(acq, stamp->view);
        }
        switch (order) {
            case MemoryOrder::RLX:
                break;
            case MemoryOrder::ACQ:
                throw RuntimeError{"store ACQ access"};
            case MemoryOrder::REL:
            case MemoryOrder::REL_ACQ:
                rel[address] = cur;
                break;
            case MemoryOrder::SEQ_CST:
                throw std::logic_error{"SEQ_CST accesses unimplemented for ra/sra"};
            default:
                throw std::logic_error{"Unimplemented memory model"};
        }
        stamp = memory->InsertAfter(stamp, address, value, rel[address], is_update);
        acq = memory->JoinViews(acq, cur);
        rel[address][address] = stamp;
    }

    Word Load(Word address, MemoryOrder order, bool is_update) {
        auto& stamp = cur[address];
        while (!memory->IsLast(address, stamp)) {
            if (!is_update || !std::next(stamp)->is_hooked) {
                if (!path_chooser->ChooseVariant({"skip", "increase"},
                                                 "increase timestamp of #" + std::to_string(address))) {
                    break;
                }
            }
            ++stamp;
            acq = memory->JoinViews(acq, stamp->view);
            switch (order) {
                case MemoryOrder::RLX:
                    break;
                case MemoryOrder::REL:
                    throw RuntimeError{"load REL access"};
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
        return stamp->value;
    }

public:
    TimestampView(std::shared_ptr<MessageMemory> memory, std::shared_ptr<PathChooser> path_chooser)
        : WithMemoryAndChooserView(std::move(memory), std::move(path_chooser)),
          rel(this->memory->Size()),
          cur(this->memory->Size()) {
        for (Word addr{}; static_cast<size_t>(addr) < cur.Size(); ++addr) {
            cur[addr] = this->memory->GetLastStamp(addr);
        }
        acq = cur;
        ReleaseFence();
    }

    Word Load(Word address, MemoryOrder order) override { return Load(address, order, false); }

    void Store(Word address, Word value, MemoryOrder order) override { Store(address, value, order, false); }

    Word Fai(Word address, Word value, MemoryOrder order) override {
        auto prev = Load(address, order, true);
        Store(address, prev + value, order, true);
        return prev;
    }

    Word Cas(Word address, Word expected, Word desired, MemoryOrder order) override {
        auto prev = Load(address, order, true);
        if (prev == expected) {
            Store(address, desired, order, true);
        }
        return prev;
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
                cur = memory->UpdateGlobalView(cur);
                break;
            default:
                throw std::logic_error{"Unimplemented memory order fence [Release/Acquire]"};
        }
    }

    void DoSilent() override { throw std::logic_error{"DoSilent in SRA"}; }

    bool HasSilent() const override { return false; }

    void Print(std::ostream& out) const override { memory->PrintView(cur, out); }
};