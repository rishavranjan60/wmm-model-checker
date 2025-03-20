#include "defs.h"
#include "tokenizer.h"
#include "parser.h"
#include "program.h"
#include "path_choosers/full.h"

#include <catch2/catch_test_macros.hpp>

#include <fstream>
#include <set>
#include <string>

namespace modelchecker_test {

template <typename T>
std::set<std::vector<int>> ProduceAllPaths(const std::vector<std::vector<T>>& elements) {
    std::vector<std::vector<int>> cur_layer{{}}, next_layer;
    for (int v = 0; v < elements.size(); ++v) {
        next_layer.clear();
        for (auto& p : cur_layer) {
            for (int e = 0; e < elements[v].size(); ++e) {
                auto new_path = p;
                new_path.push_back(e);
                next_layer.emplace_back(std::move(new_path));
            }
        }
        std::swap(cur_layer, next_layer);
    }
    return std::set(cur_layer.begin(), cur_layer.end());
}

void TestChooseThread(const std::vector<std::vector<Thread>>& threads) {
    const std::shared_ptr<Memory> null_memory;
    FullChooser chooser{};
    std::set<std::vector<int>> paths = ProduceAllPaths(threads), chosen_paths{};
    while (!chooser.Finished()) {
        std::vector<int> path;
        for (auto& t : threads) {
            path.push_back(chooser.ChooseThread(t, null_memory));
        }
        chosen_paths.emplace(std::move(path));
        chooser.NextRun();
    }
    REQUIRE(paths == chosen_paths);
}

std::vector<Thread> T(int n) {
    std::vector<Thread> threads;
    for (int i = 0; i < n; ++i) {
        threads.push_back(Thread(nullptr, std::move(std::unique_ptr<MemoryView>(nullptr)),
                                 std::shared_ptr<PathChooser>(nullptr), 0, false));
    }
    return std::move(threads);
}

std::vector<std::vector<Thread>> MakeThreads(const std::vector<int>& counts) {
    std::vector<std::vector<Thread>> threads{};
    for (auto& n : counts) {
        threads.push_back(T(n));
    }
    return std::move(threads);
}

TEST_CASE("Single step") { TestChooseThread(MakeThreads({3})); }

TEST_CASE("Single variant") { TestChooseThread(MakeThreads({1, 1, 1})); }

TEST_CASE("Multiple paths") { TestChooseThread(MakeThreads({2, 4, 3})); }

}  // namespace modelchecker_test