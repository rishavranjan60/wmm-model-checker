#pragma once

#include "memory_system/memory_view.h"
#include "memory_system/models/message_memory.h"

#include <memory>
#include <optional>

class TimestampView : public WithMemoryAndChooserView<MessageMemory> {
protected:
    MessageMemory::View view;

    virtual void ReleaseFence() {
        for (Word addr{}; static_cast<size_t>(addr) < view.size(); ++addr) {
            auto stamp = view[addr];
            view = memory->InsertAfter(stamp, addr, stamp->first, std::move(view));
        }
    }

public:
    TimestampView(std::shared_ptr<MessageMemory> memory, std::shared_ptr<PathChooser> path_chooser)
        : WithMemoryAndChooserView(std::move(memory), std::move(path_chooser)), view{} {
        view.reserve(this->memory->Size());
        for (Word addr{}; static_cast<size_t>(addr) < this->memory->Size(); ++addr) {
            view.emplace_back(this->memory->GetLastStamp(addr));
        }
    }

    Word Load(Word address, MemoryOrder order) override {
        while (!memory->IsLast(address, view[address])) {
            if (!path_chooser->ChooseVariant({"skip", "increase"},
                                             "increase timestamp of #" + std::to_string(address))) {
                break;
            }
            ++view[address];
            switch (order) {
                case MemoryOrder::RLX:
                case MemoryOrder::REL:
                    break;
                case MemoryOrder::ACQ:
                case MemoryOrder::REL_ACQ:
                case MemoryOrder::SEQ_CST:
                    view = memory->JoinViews(std::move(view), view[address]->second.view);
                    break;
                default:
                    throw std::logic_error{"Unimplemented memory model"};
            }
        }
        return view[address]->first;
    }

    void Store(Word address, Word value, MemoryOrder order) override {
        auto stamp = view[address];
        while (!memory->IsLast(address, stamp) && path_chooser->ChooseVariant({"this", "next"}, "insert after")) {
            ++stamp;
        }
        std::optional<MessageMemory::View> store_view;
        switch (order) {
            case MemoryOrder::RLX:
            case MemoryOrder::ACQ:
                break;
            case MemoryOrder::REL:
            case MemoryOrder::REL_ACQ:
            case MemoryOrder::SEQ_CST:
                store_view = std::move(view);
                break;
            default:
                throw std::logic_error{"Unimplemented memory model"};
        }
        view = memory->InsertAfter(stamp, address, value, std::move(store_view));
    }

    void Fence(MemoryOrder order) override {
        switch (order) {
            case MemoryOrder::RLX:
                break;
            case MemoryOrder::SEQ_CST:
            case MemoryOrder::REL_ACQ:
            case MemoryOrder::REL:
                ReleaseFence();
                if (order == MemoryOrder::REL) {
                    break;
                }
            case MemoryOrder::ACQ:
                for (Word addr{}; static_cast<size_t>(addr) < view.size(); ++addr) {
                    auto wrapper = view[addr]->second.view;
                    view = memory->JoinViews(std::move(view), wrapper);
                }
                break;
            default:
                throw std::logic_error{"Unimplemented memory order fence [Release/Acquire]"};
        }
    }

    void DoSilent() override { throw std::logic_error{"DoSilent in SRA"}; }

    bool HasSilent() const override { return false; }

    void Print(std::ostream& out) const override { memory->PrintView(view, out); }
};