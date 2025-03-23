#include <atomic>
#include <cassert>
#include <iostream>
#include <thread>

std::atomic<int> x{0}, y{0};
std::atomic<int> a{0}, b{0};

void thread1()
{
    std::cout << "Thread 1: Storing 1 to x\n";
    x.store(1, std::memory_order_relaxed);               // Store x first
    std::atomic_thread_fence(std::memory_order_release); // Memory fence to release
    std::cout << "Thread 1: Storing 1 to y\n";
    y.store(1, std::memory_order_relaxed); // Then store y
}

void thread2()
{
    while (y.load(std::memory_order_relaxed) != 1)
    {
        // Waiting for y to be 1
    }
    std::atomic_thread_fence(std::memory_order_acquire); // Memory fence to acquire
    std::cout << "Thread 2: Loading x value\n";
    a.store(x.load(std::memory_order_relaxed), std::memory_order_relaxed); // Store x's value
}

int main()
{
    int success_count = 0;
    int total_runs = 100; // Reducing total runs for quicker feedback

    for (int i = 0; i < total_runs; ++i)
    {
        x = 0;
        y = 0;
        a = 0;
        b = 0;

        std::thread t1(thread1);
        std::thread t2(thread2);

        t1.join();
        t2.join();

        std::cout << "Run " << i << " - a: " << a.load() << ", x: " << x.load()
                  << ", y: " << y.load() << std::endl;

        if (a == 0)
        {
            success_count++;
        }
    }

    std::cout << "PSO Violation Observed in " << success_count << " / " << total_runs << " runs.\n";
    assert(success_count > 0 && "Expected reordering under PSO");

    return 0;
}
