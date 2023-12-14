#pragma once

#include "memory_system/memory_view.h"
#include "memory_system/models/message_memory.h"
#include "memory_system/models/ra.h"

#include <memory>
#include <optional>

class GlobalTimestampView : public TimestampView {
protected:
    void ReleaseFence() override {
        for (Word addr{}; static_cast<size_t>(addr) < view.size(); ++addr) {
            auto stamp = view[addr];
            view = memory->InsertAfterLast(addr, stamp->first, std::move(view));
        }
    }
public:
    using TimestampView::TimestampView;

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
};