#pragma once

#include <atomic>
#include <vector>

class RingBuffer {
private:
    std::vector<int> m_data;
    std::vector<std::atomic<size_t>> m_seq;
    std::atomic<size_t> m_begin;
    std::atomic<size_t> m_end;

public:
    RingBuffer(size_t sz)
        : m_data(sz, 0), m_seq(sz), m_begin(0ull), m_end(0ull) {
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
        while (m_seq[idx % m_data.size()].load(std::memory_order::acquire) < idx
        )
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
        while (m_seq[idx % m_data.size()].load(std::memory_order::acquire) <
               idx + 1)
            continue;
        r = m_data[idx % m_data.size()];

        m_seq[idx % m_data.size()].store(
            idx + m_data.size(), std::memory_order::release
        );
        return true;
    }
};