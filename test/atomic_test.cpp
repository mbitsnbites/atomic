#include "atomic/atomic.h"
#include "atomic/spinlock.h"

#include "doctest.h"

#include <cstdint>
#include <thread>
#include <vector>

typedef atomic::atomic<int> atomic_int;

typedef doctest::Types<int8_t, int16_t, int32_t, int64_t> atomic_test_types;

TEST_CASE_TEMPLATE("Basic atomic<int> single threaded operation",
                   T,
                   atomic_test_types) {
  SUBCASE("atomic<> initializes to zero") {
    atomic::atomic<T> a;
    CHECK(a.load() == static_cast<T>(0));
  }

  SUBCASE("atomic<> initializes to custom value") {
    atomic::atomic<T> a(static_cast<T>(42));
    CHECK(a.load() == static_cast<T>(42));
  }

  SUBCASE("Incrementing atomic<> works as expected") {
    atomic::atomic<T> a(static_cast<T>(0));
    a.increment();
    a.increment();
    a.increment();
    CHECK(a.load() == static_cast<T>(3));
  }

  SUBCASE("Decrementing atomic<> works as expected") {
    atomic::atomic<T> a(5);
    a.decrement();
    a.decrement();
    a.decrement();
    CHECK(a.load() == static_cast<T>(2));
  }

  SUBCASE("compare_exchange with non-expected value is a no-op") {
    atomic::atomic<T> a(static_cast<T>(5));
    const bool did_swap =
        a.compare_exchange(static_cast<T>(4), static_cast<T>(9));
    CHECK(did_swap == false);
    CHECK(a.load() == static_cast<T>(5));
  }

  SUBCASE("compare_exchange with expected value performs swap") {
    atomic::atomic<T> a(static_cast<T>(5));
    const bool did_swap =
        a.compare_exchange(static_cast<T>(5), static_cast<T>(9));
    CHECK(did_swap == true);
    CHECK(a.load() == static_cast<T>(9));
  }
}

TEST_CASE("atomic<int> multi threaded operation") {
  SUBCASE("atomic<int> increments correctly with 100 threads") {
    atomic_int a;

    const int NUM_THREADS = 100;
    const int NUM_ITERATIONS = 1000;
    std::vector<std::thread> threads;
    for (int i = 0; i < NUM_THREADS; i++) {
      threads.push_back(std::thread([&a, &NUM_ITERATIONS]() {
        for (int k = 0; k < NUM_ITERATIONS; ++k) {
          a.increment();
        }
      }));
    }
    for (int i = 0; i < NUM_THREADS; i++) {
      threads[i].join();
    }

    CHECK(a.load() == (NUM_THREADS * NUM_ITERATIONS));
  }

  SUBCASE("atomic<int> decrements correctly with 100 threads") {
    atomic_int a;

    const int NUM_THREADS = 100;
    const int NUM_ITERATIONS = 1000;
    std::vector<std::thread> threads;
    for (int i = 0; i < NUM_THREADS; i++) {
      threads.push_back(std::thread([&a, &NUM_ITERATIONS]() {
        for (int k = 0; k < NUM_ITERATIONS; ++k) {
          a.decrement();
        }
      }));
    }
    for (int i = 0; i < NUM_THREADS; i++) {
      threads[i].join();
    }

    CHECK(a.load() == -(NUM_THREADS * NUM_ITERATIONS));
  }

  SUBCASE("spinlock with 100 threads") {
    atomic::spinlock lock;
    int unsafe_value = 0;

    const int NUM_THREADS = 100;
    const int NUM_ITERATIONS = 1000;
    std::vector<std::thread> threads;
    for (int i = 0; i < NUM_THREADS; i++) {
      threads.push_back(std::thread([&lock, &unsafe_value, &NUM_ITERATIONS]() {
        for (int k = 0; k < NUM_ITERATIONS; ++k) {
          // Acquire (blocking).
          lock.lock();

          // Update the unsafe value (now protected by our acquired lock).
          ++unsafe_value;

          // Release.
          lock.unlock();
        }
      }));
    }
    for (int i = 0; i < NUM_THREADS; i++) {
      threads[i].join();
    }

    CHECK(unsafe_value == (NUM_THREADS * NUM_ITERATIONS));
  }

  SUBCASE("lock_guard with 100 threads") {
    atomic::spinlock lock;
    int unsafe_value = 0;

    const int NUM_THREADS = 100;
    const int NUM_ITERATIONS = 1000;
    std::vector<std::thread> threads;
    for (int i = 0; i < NUM_THREADS; i++) {
      threads.push_back(std::thread([&lock, &unsafe_value, &NUM_ITERATIONS]() {
        for (int k = 0; k < NUM_ITERATIONS; ++k) {
          atomic::lock_guard guard(lock);

          // Update the unsafe value (now protected by our acquired lock).
          ++unsafe_value;
        }
      }));
    }
    for (int i = 0; i < NUM_THREADS; i++) {
      threads[i].join();
    }

    CHECK(unsafe_value == (NUM_THREADS * NUM_ITERATIONS));
  }
}
