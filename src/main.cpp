#include <atomic>
#include <exception>
#include <format>
#include <iostream>
#include <thread>
#include <vector>

#include "rbuffer.hpp"

void test_ring_buffer(int iterations, int num_threads) {
    RingBuffer<int> rb(num_threads * 64);
    std::atomic<int> produced{0}, consumed{0};
    std::vector<std::thread> producers, consumers;

    for (int i = 0; i < num_threads; ++i) {
        producers.emplace_back([&, iterations] {
            int local = 0;
            while (local < iterations) {
                if (rb.try_push(local))
                    ++local, ++produced;
            }
        });
    }

    for (int i = 0; i < num_threads; ++i) {
        consumers.emplace_back([&] {
            int val;
            while (consumed.load() < iterations * num_threads) {
                if (rb.try_pop(val))
                    ++consumed;
            }
        });
    }

    for (auto& t : producers)
        t.join();
    for (auto& t : consumers)
        t.join();

    if (consumed.load() != iterations * num_threads) {
        std::cerr << std::format("FAILED: {} != {}\n", consumed.load(), iterations * num_threads);
        std::exit(1);
    }
    std::cout << consumed.load() << "\n";
}

int main() try {
    size_t iters, threads;
    std::cin >> iters >> threads;
    test_ring_buffer(iters, threads);
    return 0;
} catch (std::exception& e) {
    std::cerr << std::format("unexpected exception: {}\n", e.what());
    std::exit(1);
} catch (...) {
    std::cerr << std::format("unknown exception\n");
    std::exit(1);
}
