#include <iostream>
#include <thread>
#include <atomic>

// Shared atomic variables
std::atomic<int> x{0}, y{0};
std::atomic<int> a{0}, b{0};

// Thread 1 writes to x and y with a release fence in between
void thread1() {
    x.store(1, std::memory_order_relaxed); // Relaxed store to x
    std::atomic_thread_fence(std::memory_order_release); // Release fence
    y.store(1, std::memory_order_relaxed); // Relaxed store to y
}

// Thread 2 waits for y to be 1, then reads x after an acquire fence
void thread2() {
    while (y.load(std::memory_order_relaxed) != 1) {
        // busy wait
    }
    std::atomic_thread_fence(std::memory_order_acquire); // Acquire fence
    a.store(x.load(std::memory_order_relaxed), std::memory_order_relaxed); // Store x's value into a
}

int main() {
    int success_count = 0;
    int total_runs = 100000;

    for (int i = 0; i < total_runs; ++i) {
        // Reset shared variables
        x = 0; y = 0; a = 0; b = 0;

        // Launch threads
        std::thread t1(thread1);
        std::thread t2(thread2);
        t1.join();
        t2.join();

        // Count how many times a was read as 0
        if (a == 0) {
            success_count++;
        }
    }

    // Output result
    std::cout << "PSO Violation Observed in "
              << success_count << " / " << total_runs << " runs.\n";

    return 0;
}
