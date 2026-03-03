#include <atomic>
#include <format>
#include <iostream>
#include <thread>
#include <vector>

#include "rbuffer.hpp"

void test_ring_buffer(int iterations, int num_threads) {
    RingBuffer rb(num_threads * 64);
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
        std::cout << std::format(
            "FAILED: {} != {}\n", consumed.load(), iterations * num_threads
        );
        std::exit(1337);
    }
    std::cout << "OK: " << consumed.load() << " items\n";
}

int main() {
    while (true)
        test_ring_buffer(1000, 10);
    return 0;
}
