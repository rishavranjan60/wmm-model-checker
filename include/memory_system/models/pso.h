#pragma once

#include "memory_system/memory.h"
#include "memory_system/memory_view.h"
#include "memory_system/memory.h"


#include <deque>
#include <ranges>
#include <unordered_map>

class WriteBufferPerLocView : public WithMemoryAndChooserView<ArrayMemory> {
 private:
  std::unordered_map<Word, std::deque<Word>> buffers;

  void Propagate(Word address) {
    if (!buffers.contains(address)) {
      throw std::logic_error{
          "Trying to propagate address with empty buffer [PSO]"};
    }
    memory->Store(address, buffers[address].back());
    buffers[address].pop_back();
    if (buffers[address].empty()) {
      buffers.erase(address);
    }
  }

 public:
  using WithMemoryAndChooserView::WithMemoryAndChooserView;

  void Store(Word address, Word value, MemoryOrder) override {
    memory->CheckAddress(address);
    buffers[address].emplace_front(value);
  }

  Word Load(Word address, MemoryOrder) override {
    if (buffers.contains(address)) {
      return buffers[address].front();
    }
    return memory->Load(address);
  }

  Word Fai(Word address, Word value, MemoryOrder) override {
    auto res = MemoryView::Fai(address, value, MemoryOrder::SEQ_CST);
    while (buffers.contains(address)) {
      Propagate(address);
    }
    return res;
  }

  void Fence(MemoryOrder) override {
    while (!buffers.empty()) {
      Propagate(buffers.begin()->first);
    }
  }

  void DoSilent() override {
    if (buffers.empty()) {
      throw std::logic_error{"DoSilent with empty buffers [PSO]"};
    }
    std::vector<std::string> variants;
    variants.reserve(buffers.size());
    std::vector<Word> addresses;
    addresses.reserve(buffers.size());
    for (const auto &[addr, buffer] : buffers) {
      variants.emplace_back('#' + std::to_string(addr));
      addresses.emplace_back(addr);
    }
    int res = path_chooser->ChooseVariant(variants, "address for propagation");
    Propagate(addresses[res]);
  }

  bool HasSilent() const override { return !buffers.empty(); }

  void Print(std::ostream &out) const override {
    for (const auto &[address, buffer] : buffers) {
      out << '#' << address;
      for (const auto &value : buffer | std::views::reverse) {
        out << " <- " << value;
      }
      out << '\n';
    }
  }
};
