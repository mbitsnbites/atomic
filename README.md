## ⚠️ This repository has moved to: https://gitlab.com/mbitsnbites/atomic

# Atomic operations for C++

This is a very lightweight library that implements thread safe, atomic
operations for C++. It is portable and easy to use.


## CI status
OS | master branch
--- | ---
Linux & macOS (GCC, Clang) | [![Build status](https://travis-ci.org/mbitsnbites/atomic.svg?branch=master)](https://travis-ci.org/mbitsnbites/atomic)
Windows (MSVC) | [![Build status](https://ci.appveyor.com/api/projects/status/w502ka3k674wn25f?svg=true)](https://ci.appveyor.com/project/mbitsnbites/atomic/branch/master)


## Why avoid std::atomic?

Because `<atomic>` drags in loads of code in the pre-processing step (over 1MB
of code for MSVC 2015). This library, which is based on compiler intrinsics
(and a fallback to std::atomic), is much lighter.

Another reason may be that you can not (for whatever reason) use C++11 in your
project, but still need portable atomic operations.


## Example code

### Spinlock with an automatic lock guard

```c++
#include "atomic/spinlock.h"

static atomic::spinlock lock;

void foo() {
  atomic::lock_guard guard(lock);

  // Stuff that is synchronized by the lock...
}
```

This is the generated machine code (clang 3.8, x86_64):

```assembly
foo:
    movl            $1, %ecx
.spin:
    xorl            %eax, %eax
    lock cmpxchgl   %ecx, lock(%rip)
    jne             .spin

    // Stuff that is synchronized by the lock...

    xorl            %eax, %eax
    xchgl           %eax, lock(%rip)
    retq
```

## License

This is free and unencumbered software released into the public domain.

For more info, see [LICENSE](LICENSE).

