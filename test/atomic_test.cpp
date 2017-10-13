#include "atomic/atomic.h"
#include "atomic/spinlock.h"

#include "doctest.h"

#include <thread>
#include <vector>

typedef atomic::atomic<int> atomic_int;

TEST_CASE("Basic atomic<int> single threaded operation") {
  SUBCASE("atomic<int> initializes to zero") {
    atomic_int a;
    CHECK(a.load() == 0);
  }

  SUBCASE("atomic<int> initializes to custom value") {
    atomic_int a(42);
    CHECK(a.load() == 42);
  }

  SUBCASE("Incrementing atomic<int> works as expected") {
    atomic_int a(0);
    a.increment();
    a.increment();
    a.increment();
    CHECK(a.load() == 3);
  }

  SUBCASE("Decrementing atomic<int> works as expected") {
    atomic_int a(2);
    a.decrement();
    a.decrement();
    a.decrement();
    CHECK(a.load() == -1);
  }

  SUBCASE("compare_exchange with non-expected value is a no-op") {
    atomic_int a(5);
    const bool did_swap = a.compare_exchange(4, 9);
    CHECK(did_swap == false);
    CHECK(a.load() == 5);
  }

  SUBCASE("compare_exchange with expected value performs swap") {
    atomic_int a(5);
    const bool did_swap = a.compare_exchange(5, 9);
    CHECK(did_swap == true);
    CHECK(a.load() == 9);
  }
}

TEST_CASE("atomic<int> multi threaded operation") {
  SUBCASE("atomic<int> increments correctly with 100 threads") {
    atomic_int a;

    const int NUM_THREADS = 100;
    const int NUM_ITERATIONS = 1000;
    std::vector<std::thread> threads;
    for (int i = 0; i < NUM_THREADS; i++) {
      threads.push_back(std::thread([&a]() {
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
      threads.push_back(std::thread([&a]() {
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
      threads.push_back(std::thread([&lock, &unsafe_value]() {
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
      threads.push_back(std::thread([&lock, &unsafe_value]() {
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
