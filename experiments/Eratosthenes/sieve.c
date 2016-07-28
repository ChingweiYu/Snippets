#include "sieve.h"

#include <math.h>
#include <string.h>

#include <iostream>

static const uint32_t l1d_cache_size = 1 << 15;

void sieve(uint32_t n, uint8_t prime[]) {
    memset(prime, 1, n >> 1);
    uint32_t bound = sqrt(n);
    for (uint32_t i = 3; i <= bound; i += 2) {
        if (prime[number_to_index(i)]) {
            for (uint32_t j = i * i; j <= n; j += (i << 1)) {
                prime[number_to_index(j)] = 0;
            }
        }
    }
}

void improved_sieve(uint32_t n, uint8_t prime[]) {
    memset(prime, 1, n >> 1);
    uint32_t bound = sqrt(n);
    for (uint32_t i = 3; i <= bound; i += 2) {
        if (prime[number_to_index(i)]) {
            for (uint32_t k = n / i - (((n / i) & 1) == 0), j = i * k; 
                         k >= i; k -= 2, j -= (i << 1) ) {
                if (prime[number_to_index(k)]) {
                    prime[number_to_index(j)] = 0;
                }
            }
        }
    }
}

// The upper bound of the number of primes at most n.
// This formula is from the following paper:
// Pierre Dusart, 
// The $k^{th}$ prime is greater than $k(\ln k +\ln\ln k -1)$ for $k\geq 2$,
// Mathematics of Computation, 8(225): 411-415 (1999)
uint32_t upper_bound_of_pi(uint32_t n) {
    return (n / log(n)) * (1 + 1.2762 / log(n));
}

void linear_sieve(uint32_t n, uint8_t prime[]) {
    memset(prime, 1, n >> 1);
    uint32_t bound = sqrt(n);
    sieve(bound, prime);
    uint32_t primes[upper_bound_of_pi(bound)], number_of_primes = 0;
    for (uint32_t i = 3; i <= bound; i += 2)
        if (prime[number_to_index(i)])
            primes[number_of_primes++] = i;
    for (uint32_t i = 3; i <= n / 3; i += 2) {
        for (uint32_t j = 0; j < number_of_primes && primes[j] <= n / i; ++j) {
            prime[number_to_index(primes[j] * i)] = 0;
            if (i % primes[j] == 0)
                break;
        }
    }
}

void segmented_sieve(uint32_t n, uint8_t prime[]) {
    memset(prime, 1, n >> 1);
    uint32_t bound = sqrt(n);
    sieve(bound, prime);
    uint32_t primes[upper_bound_of_pi(bound)], number_of_primes = 0;
    for (uint32_t i = 3; i <= bound; i += 2)
        if (prime[number_to_index(i)])
            primes[number_of_primes++] = i;
    uint32_t next[number_of_primes], s = 0;
    uint32_t segment_size = l1d_cache_size;
    for (uint32_t low = bound + 1, 
                  high = low + segment_size > n ? n : low + segment_size;
         low <= n; low += segment_size) {
        for (uint32_t h = 0; h < number_of_primes; ++h) {
            uint32_t i = primes[h], j;
            if (i * i > high)
                break;
            if (h < s) {
                j = next[h];
            } else {
                j = ((low + (i - 1)) / i) * i;
                j += i * ((j & 1) == 0);
                ++s;
            }
            for (; j <= high; j += (i << 1)) {
                prime[number_to_index(j)] = 0;
            }
            next[h] = j;
        }
        high = (high + segment_size > n) ? n : high + segment_size;
    }
}

// Number of bytes having at least n + 1 bits
static inline uint64_t bytes(uint64_t n) {
    return (n >> 4) + (((n >> 1) & 7) != 0);
}

static inline void bit_reset(uint64_t x, uint32_t bitset[]);
void bit_reset(uint64_t x, uint32_t bitset[]) { 
    bitset[x>>5] &= ~(1 << (x&31));
}

void sieve_bit(uint64_t n, uint32_t prime[]) {
    memset(prime, 0xFF, bytes(n));
    uint64_t bound = sqrtl(n);
    uint64_t max_index = number_to_bit_index(n);
    for (uint64_t i = 3, index = 0; i <= bound; i += 2, ++index) {
        if (bit_get(index, prime)) {
            for (uint64_t j_index = number_to_bit_index(i * i);
            j_index <= max_index; j_index += i) {
                bit_reset(j_index, prime);
            }
        }
    }
}

void improved_sieve_bit(uint64_t n, uint32_t prime[]) {
    memset(prime, 0xFF, bytes(n));
    uint64_t bound = sqrtl(n);
    for (uint64_t i = 3, index = 0; i <= bound; i += 2, ++index) {
        if (bit_get(index, prime)) {
            uint64_t k = n / i - ((n / i) % 2 == 0);
            uint64_t j_index = number_to_bit_index(i * k);
            for (int64_t k_index = number_to_bit_index(k);
                 k_index >= (int64_t)index; j_index -= i, --k_index) {
                if (bit_get(k_index, prime))
                    bit_reset(j_index, prime);
            }
        }
    }
}

void linear_sieve_bit(uint64_t n, uint32_t prime[]) {
    memset(prime, 0xFF, bytes(n));
    uint64_t bound = sqrtl(n);
    sieve_bit(bound, prime);
    uint32_t primes[upper_bound_of_pi(bound)], number_of_primes = 0;
    for (uint32_t i = 3, index = 0; i <= bound; i += 2, ++index)
        if (bit_get(index, prime))
            primes[number_of_primes++] = i;
    for (uint64_t i = 3; i <= n / 3; i += 2) {
        for (uint32_t j = 0; j < number_of_primes && primes[j] <= n / i; ++j) {
            bit_reset(number_to_bit_index(primes[j] * i), prime);
            if (i % primes[j] == 0)
                break;
        }
    }
}

void segmented_sieve_bit(uint64_t n, uint32_t prime[]) {
    memset(prime, 0xFF, bytes(n));
    uint64_t bound = sqrtl(n);
    sieve_bit(bound, prime);
    uint32_t primes[upper_bound_of_pi(bound)], number_of_primes = 0;
    for (uint32_t i = 3, index = 0; i <= bound; i += 2, ++index)
        if (bit_get(index, prime))
            primes[number_of_primes++] = i;
    uint64_t next[number_of_primes], s = 0;
    uint64_t segment_size = l1d_cache_size << 7;
    for (uint64_t low = bound + 1, 
                  high = low + segment_size > n ? n : low + segment_size;
         low <= n; low += segment_size) {
        uint64_t high_index = number_to_bit_index(high);
        for (uint64_t h = 0; h < number_of_primes; ++h) {
            uint64_t i = primes[h], j_index;
            if (i * i > high)
                break;
            if (h < s) {
                j_index = next[h];
            } else {
                uint64_t j = ((low + (i - 1)) / i) * i;
                j += i * ((j & 1) == 0);
                j_index = number_to_bit_index(j);
                ++s;
            }
            for (; j_index <= high_index; j_index += i) {
                bit_reset(j_index, prime);
            }
            next[h] = j_index;
        }
        high = (high + segment_size > n) ? n : high + segment_size;
    }
}

static const uint8_t steps[] = {6, 4, 2, 4, 2, 4, 6, 2};
static const uint8_t number_of_coprimes = 8;

static inline uint8_t next_step_index(uint8_t index) {
    return ++index & 7;
}

void wheel_bit(uint64_t n, uint32_t prime[]) {
    uint64_t bound = sqrtl(n);
    uint64_t max_index = wheel_index(n);
    memset(prime, 0xFF, (max_index >> 3) + 1);

    uint8_t step_index = 1;
    for (uint64_t i = steps[0] + 1, index = 1; i <= bound; ++index) {
        if (!bit_get(index, prime))
            continue;
        uint64_t m = i;
        uint64_t j_index = wheel_index(i * m), k_index = j_index;
        uint64_t increment[number_of_coprimes];
        uint8_t m_step_index = step_index;
        for (uint8_t c = 0; c < number_of_coprimes; ++c) {
            m += steps[m_step_index];
            uint64_t temp = wheel_index(i * m);
            increment[m_step_index] = temp - k_index;
            m_step_index = next_step_index(m_step_index);
            k_index = temp;
        }
        for (m_step_index = step_index; j_index <= max_index; ) {
            bit_reset(j_index, prime);
            j_index += increment[m_step_index];
            m_step_index = next_step_index(m_step_index);
        }
        i += steps[step_index];
        step_index = next_step_index(step_index);
    }
}

static const uint8_t coprime_index[] = {
    8, 0, 8, 8, 8, 8, 8, 1, 8, 8, 
    8, 2, 8, 3, 8, 8, 8, 4, 8, 5, 
    8, 8, 8, 6, 8, 8, 8, 8, 8, 7};

void segmented_wheel_bit(uint64_t n, uint32_t prime[]) {
    uint64_t bound = sqrtl(n);
    uint64_t max_index = wheel_index(n);
    memset(prime, 0xFF, (max_index >> 3) + 1);

    wheel_bit(bound, prime);
    uint32_t primes[upper_bound_of_pi(bound)], number_of_primes = 0;
    uint8_t step_index = 1;
    for (uint32_t i = 7, index = 1; index <= wheel_index(bound); ++index)  {
        if (bit_get(index, prime)) {
            primes[number_of_primes++] = i;
        }
        i += steps[step_index];
        step_index = next_step_index(step_index);
    }    
    uint64_t next[number_of_primes], s = 0;
    uint8_t next_step[number_of_primes];
    uint64_t increments[number_of_primes][number_of_coprimes];
    uint64_t segment_size = l1d_cache_size << 7;
    for (uint64_t low = bound + 1, 
                  high = low + segment_size > n ? n : low + segment_size;
         low <= n; low += segment_size) {
        uint64_t high_index = number_to_bit_index(high);
        for (uint32_t h = 0; h < number_of_primes; ++h) {
            uint64_t i = primes[h], j_index;
            if (i * i > high)
                break;
            if (h < s) {
                j_index = next[h];
                step_index = next_step[h];
            } else {
                uint64_t m = ((low + (i - 1)) / i), j = m * i;
                for (; coprime_index[j % 30] > 7; ++m) {
                    j += i;
                }
                step_index = coprime_index[m % 30];
                j_index = wheel_index(j);
                uint8_t m_step_index = step_index;
                uint64_t k_index = wheel_index(j);
                for (uint8_t c = 0; c < number_of_coprimes; ++c) {
                    m += steps[m_step_index];
                    uint64_t temp = wheel_index(i * m);
                    increments[h][m_step_index] = temp - k_index;
                    m_step_index = next_step_index(m_step_index);
                    k_index = temp;
                }
                ++s;
            }
            while (j_index <= high_index) {
                bit_reset(j_index, prime);
                j_index += increments[h][step_index];
                step_index = next_step_index(step_index);
            }
            next[h] = j_index;
            next_step[h] = step_index;
        }
        high = (high + segment_size > n) ? n : high + segment_size;
    }
}
