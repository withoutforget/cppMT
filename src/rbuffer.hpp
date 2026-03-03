#pragma once

#include <atomic>
#include <new>
#include <vector>

class RingBuffer {
protected:
    using mo = std::memory_order;

    struct alignas(std::hardware_destructive_interference_size) padded_atomic {
        std::atomic<size_t> value;

        operator std::atomic<size_t>&() noexcept { return value; }
    };

protected:
    std::vector<int> m_data;
    std::vector<padded_atomic> m_seq;
    std::atomic<size_t> m_begin;
    std::atomic<size_t> m_end;

    size_t get_vector_index(size_t pos) const noexcept { return pos % m_data.size(); }

    void wait_for_sequence_update(std::atomic<size_t>& seq, size_t at_least_value) {
        while (seq.load(mo::acquire) < at_least_value)
            continue;
    }

    bool try_claim_slot(std::atomic<size_t>& slot, size_t& expected) const noexcept {
        return slot.compare_exchange_strong(expected, expected + 1, mo::acq_rel, mo::relaxed);
    }

    bool is_full(size_t from, size_t to) const noexcept { return to - from >= m_data.size(); }

    bool is_empty(size_t idx) const noexcept { return idx >= m_end.load(mo::acquire); }

public:
    RingBuffer(size_t sz) : m_data(sz, 0), m_seq(sz), m_begin(0ull), m_end(0ull) {
        for (size_t i = 0; i < sz; i++)
            m_seq[i].value.store(i, mo::release);
    }
    bool try_push(int v) {
        size_t idx = m_end.load(mo::acquire);
        size_t begin = m_begin.load(mo::acquire);

        const size_t vec_index = get_vector_index(idx);
        int& data = m_data[vec_index];
        std::atomic<size_t>& sequence = m_seq[vec_index];

        if (is_full(idx, begin))
            return false;
        if (!try_claim_slot(m_end, idx))
            return false;

        wait_for_sequence_update(sequence, idx);

        data = v;
        sequence.store(idx + 1, mo::release);

        return true;
    }
    bool try_pop(int& r) {
        size_t idx = m_begin.load(mo::acquire);

        const size_t vec_index = get_vector_index(idx);
        int& data = m_data[vec_index];
        std::atomic<size_t>& sequence = m_seq[vec_index];

        if (is_empty(idx))
            return false;
        if (!try_claim_slot(m_begin, idx))
            return false;

        wait_for_sequence_update(sequence, idx + 1);

        r = data;
        sequence.store(idx + m_data.size(), mo::release);

        return true;
    }
};