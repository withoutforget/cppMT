#include <atomic>
#include <format>
#include <iostream>
#include <thread>
#include <vector>


class RingBuffer {
private:
    std::vector<int> m_data;
    std::vector<std::atomic<size_t>> m_seq;
    std::atomic<size_t> m_begin;
    std::atomic<size_t> m_end;

public:
    RingBuffer(size_t sz) : m_data(sz, 0), m_seq(sz), m_begin(0ull), m_end(0ull) {
        for (size_t i = 0; i < sz; i++)
            m_seq[i] = i;
    }
    bool try_push(int v) {
        size_t idx = m_end.load(std::memory_order::acquire);
        size_t begin = m_begin.load(std::memory_order::acquire);

        if (idx - begin >= m_data.size())
            return false;

        if (!m_end.compare_exchange_strong(
                idx,
                idx + 1,
                std::memory_order::acq_rel,
                std::memory_order::relaxed
            ))
            return false;
        while (m_seq[idx % m_data.size()].load(std::memory_order::acquire) < idx)
            continue;
        m_data[idx % m_data.size()] = v;
        m_seq[idx % m_data.size()].store(idx + 1, std::memory_order::release);
        return true;
    }
    bool try_pop(int& r) {
        size_t idx = m_begin.load(std::memory_order::acquire);
        if (idx >= m_end.load(std::memory_order::acquire))
            return false;

        if (!m_begin.compare_exchange_strong(
                idx,
                idx + 1,
                std::memory_order::acq_rel,
                std::memory_order::relaxed
            ))
            return false;
        while (m_seq[idx % m_data.size()].load(std::memory_order::acquire) < idx + 1)
            continue;
        r = m_data[idx % m_data.size()];

        m_seq[idx % m_data.size()].store(idx + m_data.size(), std::memory_order::release);
        return true;
    }
};

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