#ifndef ATOMIC_H_
#define ATOMIC_H_

#if defined(__GNUC__) || defined(__clang__) || defined(__xlc__)
#define ATOMIC_USE_GCC_INTRINSICS
#elif defined(_MSC_VER)
#define ATOMIC_USE_MSVC_INTRINSICS
#include <intrin.h>
#pragma intrinsic(_InterlockedIncrement)
#pragma intrinsic(_InterlockedDecrement)
#pragma intrinsic(_InterlockedCompareExchange)
#pragma intrinsic(_InterlockedExchange)
#else
#define ATOMIC_USE_CPP11_ATOMIC
#include <atomic>
#endif

namespace atomic {
template <typename T>
class atomic {
public:
  atomic() : value_(0) {}

  explicit atomic(const T value) : value_(value) {}

  /// @brief Performs an atomic increment operation (value + 1).
  /// @returns The new value of the atomic object.
  T increment() {
#if defined(ATOMIC_USE_GCC_INTRINSICS)
    return __atomic_add_fetch(&value_, 1, __ATOMIC_SEQ_CST);
#elif defined(ATOMIC_USE_MSVC_INTRINSICS)
    return _InterlockedIncrement(&value_);
#else
    return ++value_;
#endif
  }

  /// @brief Performs an atomic decrement operation (value - 1).
  /// @returns The old value of the atomic object.
  T decrement() {
#if defined(ATOMIC_USE_GCC_INTRINSICS)
    return __atomic_sub_fetch(&value_, 1, __ATOMIC_SEQ_CST);
#elif defined(ATOMIC_USE_MSVC_INTRINSICS)
    return _InterlockedDecrement(&value_);
#else
    return --value_;
#endif
  }

  /// @brief Performs an atomic compare-and-swap (CAS) operation.
  ///
  /// The value of the atomic object is only updated to the new value if the
  /// old value of the atomic object matches @c expected_val.
  ///
  /// @param expected_val The expected value of the atomic object.
  /// @param new_val The new value to write to the atomic object.
  /// @returns True if new_value was written to the atomic object.
  bool compare_and_swap(const T expected_val, const T new_val) {
#if defined(ATOMIC_USE_GCC_INTRINSICS)
    T e = expected_val;
    return __atomic_compare_exchange_n(
        &value_, &e, new_val, true, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
#elif defined(ATOMIC_USE_MSVC_INTRINSICS)
    const T old_val =
        _InterlockedCompareExchange(&value_, new_val, expected_val);
    return (old_val == expected_val);
#else
    T e = expected_val;
    return value_.compare_exchange_weak(e, new_val);
#endif
  }

  /// @brief Performs an atomic set operation.
  ///
  /// The value of the atomic object is unconditionally updated to the new
  /// value.
  ///
  /// @param new_val The new value to write to the atomic object.
  void set(const T new_val) {
#if defined(ATOMIC_USE_GCC_INTRINSICS)
    return __atomic_store_n(&value_, new_val, __ATOMIC_SEQ_CST);
#elif defined(ATOMIC_USE_MSVC_INTRINSICS)
    (void)_InterlockedExchange(&value_, new_val);
#else
    return value_.store(new_val);
#endif
  }

  /// @returns the current value of the atomic object.
  /// @note Be careful about how this is used, since any operations on the
  /// returned value are inherently non-atomic.
  T value() const {
#if defined(ATOMIC_USE_GCC_INTRINSICS)
    return __atomic_load_n(&value_, __ATOMIC_SEQ_CST);
#elif defined(ATOMIC_USE_MSVC_INTRINSICS)
    // TODO(m): Is there a better solution for MSVC?
    return value_;
#else
    return value_;
#endif
  }

private:
#if defined(ATOMIC_USE_GCC_INTRINSICS) || defined(ATOMIC_USE_MSVC_INTRINSICS)
  volatile T value_;
#else
  std::atomic<T> value_;
#endif
};

}  // namespace atomic

// Undef temporary defines.
#undef ATOMIC_USE_GCC_INTRINSICS
#undef ATOMIC_USE_MSVC_INTRINSICS
#undef ATOMIC_USE_CPP11_ATOMIC

#endif  // ATOMIC_H_
