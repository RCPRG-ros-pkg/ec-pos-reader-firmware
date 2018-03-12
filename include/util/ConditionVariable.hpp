#pragma once

namespace util {

class ConditionVariable
{
public:
	template <typename TLock>
	void wait(TLock& lock)
	{
		static_cast<void>(lock);

		// Wait for interrupt instruction, no need to unlock (enable interrupts).
		// The WFI instruction will exit when there is a pending interrupt
		// even if all the interrupts are masked
		__asm(
			"    wfi\n"
		);
	}

	void notify()
	{
	    // No need to do anything
	}
};

} // namespace util
