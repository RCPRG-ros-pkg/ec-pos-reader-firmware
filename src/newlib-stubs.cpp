/**
 * newlib-stub.cpp
 * File with missing syscalls for newlib
 * Author: akowalew
 */

#include <algorithm>

#include <cstdlib>
#include <sys/unistd.h>

extern "C" {

#include <errno.h>
#undef errno
int errno;

/*
 * @brief exit minimal implementation
 * @details Never exits.
 * It is best to place a breakpoint inside it to catch it
 *
 * @param status
*/
void _exit(int status)
{
    static_cast<void>(status);
    while(1)
    {
    }
}

/**
 * @brief Send a kill signal, minimal implementation
 * @details
 *
 * @param pid
 * @param sig
 *
 * @return Returns -1 and sets errno to EINVAL
 */
// int _kill(int pid, int sig)
// {
//     static_cast<void>(pid);
//     static_cast<void>(sig);
//     errno = EINVAL;
//     return (-1);
// }

/**
 * @brief Get Process ID minimal implementation
 * @details
 * @return Returns 1
 */
// int _getpid()
// {
//     return 1;
// }

/**
 * @brief Close a file. Minimal implementation
 * @details
 *
 * @param file
 * @return Returns -1
 */
// int _close(int file)
// {
//     static_cast<void>(file);
//     return -1;
// }

/**
 * @brief Status of an open file. Minimal implementation
 * @details Configures all files as special character devices
 *
 * @param file
 * @param stat
 *
 * @return Returns 0
 */
// #include <sys/stat.h>
// int _fstat(int file, struct stat *st)
// {
//     static_cast<void>(file);
//     st->st_mode = S_IFCHR;
//     return 0;
// }

/**
 * @brief Query whether output stream is a terminal. Minimal implementation
 * @details
 *
 * @param file
 * @return Always returns 1 - output stream is a terminal
 */
// int _isatty(int file)
// {
//     static_cast<void>(file);
// 	switch (file){
// 	    case STDOUT_FILENO:
// 	    case STDERR_FILENO:
// 	    case STDIN_FILENO:
// 	        return 1;
// 	    default:
// 	        errno = EBADF;
// 	        return 0;
//     }
// }

/**
 * @brief Set position in a file. Minimal implementation
 * @details
 *
 * @param file
 * @param ptr
 * @param dir
 * @return Always returns 0
 */
// int _lseek(int file, int ptr, int dir)
// {
//     static_cast<void>(file);
//     static_cast<void>(ptr);
//     static_cast<void>(dir);
//     return 0;
// }

// /**
//  * @brief Read from a file. Minimal implementation
//  * @details
//  *
//  * @param file
//  * @param ptr
//  * @param len
//  * @return Always returns 0
//  */
// int _read(int file, char *ptr, int len)
// {
// 	if(file != STDIN_FILENO)
// 	{
// 		errno = EBADF;
// 		return -1;
// 	}

// 	std::generate(ptr, ptr + len, tty::read);
// 	return len;
// }

// /**
//  * @brief Write to a file. Minimal implementation
//  * @details
//  *
//  * @param file
//  * @param ptr
//  * @param len
//  * @return Returns number of characters written
//  */
// int _write(int file, const char *ptr, int len)
// {
//     if(file != STDOUT_FILENO && file != STDERR_FILENO)
//     {
//     	errno = EBADF;
//     	return -1;
//     }

//     std::for_each(ptr, ptr + len, tty::write);
//     return len;
// }

// /**
//  * @brief Increase program data space. Minimal implementation
//  * @details
//  *
//  * @param incr size of increase
//  * @return Returns address of begin of increased data space
//  */
// char *heap_end;
// caddr_t _sbrk(int incr)
// {
//     extern char __end__;     /* Defined by the linker */
//     register char * stack_ptr asm ("sp");
//     char *prev_heap_end;

//     if(heap_end == nullptr)
//     {
//          heap_end = &__end__;
//     }

//     prev_heap_end = heap_end;
//     if(heap_end + incr > stack_ptr)
//     {
//         _write(1, "Heap and stack collision\n", 25);
//         abort();
//     }

//     heap_end += incr;
//     return (caddr_t) prev_heap_end;
// }

}
