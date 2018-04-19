#pragma once

namespace app {
namespace common {

struct Mutex
{
    void lock()
    {
        __asm(
            "cpsid  i\n" // disable interrupts
        );
    }

    void unlock()
    {
        __asm(
            "cpsie  i\n" // enable interrupts
        );
    }

    void lockInterruptCtx()
    {
        // All interrupts are already disabled, no need to do anything
    }

    void unlockInterruptCtx()
    {
    }
};

} // namespace common
} // namespace app
