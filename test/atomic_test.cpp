#include "atomic/atomic.h"

#include "doctest.h"

#include <thread>
#include <vector>

typedef atomic::atomic<int> atomic_int;

TEST_CASE("Basic atomic<int> single threaded operation") {
  SUBCASE("atomic<int> initializes to zero") {
    atomic_int a;
    CHECK(a.value() == 0);
  }

  SUBCASE("atomic<int> initializes to custom value") {
    atomic_int a(42);
    CHECK(a.value() == 42);
  }

  SUBCASE("incrementing atomic<int> works as expected") {
    atomic_int a(0);
    a.increment();
    a.increment();
    a.increment();
    CHECK(a.value() == 3);
  }

  SUBCASE("decrementing atomic<int> works as expected") {
    atomic_int a(2);
    a.decrement();
    a.decrement();
    a.decrement();
    CHECK(a.value() == -1);
  }

  SUBCASE("compare_and_swap with non-expected value is a no-op") {
    atomic_int a(5);
    const bool did_swap = a.compare_and_swap(4, 9);
    CHECK(did_swap == false);
    CHECK(a.value() == 5);
  }

  SUBCASE("compare_and_swap with expected value performs swap") {
    atomic_int a(5);
    const bool did_swap = a.compare_and_swap(5, 9);
    CHECK(did_swap == true);
    CHECK(a.value() == 9);
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

    CHECK(a.value() == (NUM_THREADS * NUM_ITERATIONS));
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

    CHECK(a.value() == -(NUM_THREADS * NUM_ITERATIONS));
  }

  SUBCASE("manual spin-lock with atomic<int> with 100 threads") {
    atomic_int a;
    int usafe_value = 0;

    const int NUM_THREADS = 100;
    const int NUM_ITERATIONS = 1000;
    std::vector<std::thread> threads;
    for (int i = 0; i < NUM_THREADS; i++) {
      threads.push_back(std::thread([&a, &usafe_value]() {
        for (int k = 0; k < NUM_ITERATIONS; ++k) {
          // Acquire (blocking).
          while (!a.compare_and_swap(0, 1))
            ;

          // Update unsafe value (now protected by our acquired lock).
          ++usafe_value;

          // Release.
          a.set(0);
        }
      }));
    }
    for (int i = 0; i < NUM_THREADS; i++) {
      threads[i].join();
    }

    CHECK(usafe_value == (NUM_THREADS * NUM_ITERATIONS));
  }
}
