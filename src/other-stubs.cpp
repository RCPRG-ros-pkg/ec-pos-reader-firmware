/**
 * other-stubs.cpp
 * Contains replacements for stdc/stdc++ library functions
 * which causes code bloat due to e.g. heap usage
 * Author: akowalew
 */

#include <cstdint>
#include <cstdlib>
#include "tivaware/utils/uartstdio.h"

extern "C"
__attribute__((__noinline__))
void abort()
{
    while(1);
}

// //! Prevents to generate unwind code, what occurs when calling stdlib functions
// extern "C"
// void __aeabi_unwind_cpp_pr1 (void)
// {
// 	abort();
// }

// //! Prevents to generate unwind code, what occurs when calling stdlib functions
// extern "C"
// void __aeabi_unwind_cpp_pr0 (void)
// {
// 	abort();
// }

//! Does exactly the same as the standard function, but doesn't use classic
//! printf, because it requires using heap and malloc. Instead, use UARTprintf
extern "C"
void __assert_func(const char* filename, int lineno,
	const char* func, const char* expression)
{
	UARTprintf("Assertion failed: %s, file %s, line %d, function: %s\n",
		expression, filename, lineno, func);

	abort();
}

// void *operator new(size_t) { abort(); }
// void *operator new[](size_t) { abort(); }
// void operator delete(void *) noexcept { abort(); }
// void operator delete(void *, std::size_t) noexcept { abort(); }
// void operator delete[](void *) noexcept { abort(); }
// void operator delete[](void *, std::size_t) noexcept { abort(); }
// void *__cxa_pure_virtual = nullptr;
