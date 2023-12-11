#pragma once

#include "memory_system/memory_view.h"
#include "memory_system/models/message_memory.h"

#include <memory>
#include <optional>

class GlobalTimestampView : public WithMemoryAndChooserView<MessageMemory> {
private:
    MessageMemory::View view;

public:
    GlobalTimestampView(std::shared_ptr<MessageMemory> memory, std::shared_ptr<PathChooser> path_chooser)
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
        view = memory->InsertAfterLast(address, value, std::move(store_view));
    }

    void Fence(MemoryOrder) override {
        // TODO
    }

    void DoSilent() override { throw std::logic_error{"DoSilent in SRA"}; }

    bool HasSilent() const override { return false; }

    void Print(std::ostream& out) const override {
        memory->PrintView(view, out);
    }
};