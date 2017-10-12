# Atomic operations for C++

This is a very lightweight library that implements thread safe, atomic
operations for C++. It is portable and easy to use.


## Why avoid std::atomic?

Because `<atomic>` drags in loads of code in the pre-processing step (over 1MB
of code for MSVC 2015). This library, which is based on compiler intrinsics
(and a fallback to std::atomic), is much lighter.

Another reason may be that you can not (for whatever reason) C++11 in your
project, but still need portable atomic operations.


## License

This is free and unencumbered software released into the public domain.

For more info, see [LICENSE](LICENSE).

