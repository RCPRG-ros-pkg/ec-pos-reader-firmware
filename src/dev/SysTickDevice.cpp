#include "dev/SysTickDevice.hpp"

#include <cassert>

extern int clockHz;

namespace dev {

SysTickDevice* SysTickDevice::_instance = nullptr;

class SysTickGuard
{
public:
    SysTickGuard() noexcept
    {
        HWREG(NVIC_ST_CTRL) &= ~(NVIC_ST_CTRL_INTEN); // disable SysTick interrupts
        __asm("dsb"); // data synchronization barrier
        __asm("isb"); // instruction synchronization barrier
    }

    ~SysTickGuard() noexcept
    {
        HWREG(NVIC_ST_CTRL) |= NVIC_ST_CTRL_INTEN; // enable SysTick interrupts
    }
};

/**
 * @brief Constructor
 * @details
 */
SysTickDevice::SysTickDevice()
{
	assert(_instance == nullptr);
	_instance = this;

	SysTickIntRegister(sysTickISR);
    // SysTick interrupts is enabled after call to SysTickIntRegister

	const std::uint32_t period = (clockHz / TickFrequency);
	SysTickPeriodSet(period);
}

/**
 * @brief Destructor
 * @details
 */
SysTickDevice::~SysTickDevice()
{
    // SysTickIntUnregister disables systick interrupts first
    // so it acts like a critical section, because we are in EventLoopContext
    SysTickIntUnregister();
    assert(_instance);
    _instance = nullptr;

    // if device was active waiting before destructing, disable it now
    if(_state == State::Active)
    {
        SysTickDisable();
    }
}

void SysTickDevice::startWait_BothCtxs(WaitUnitTimeType waitTime)
{
    // Before startWait_EventLoopCtx, cancelWait_EventLoopCtx will be called
    // so no critical section needed. This same in interrupt context.
    assert(_state == State::Stopped);

    _waitingDuration = _remainingDuration = WaitTimeUnitDuration(waitTime);
    _state = State::Active;
    SysTickClear();
    SysTickEnable();
}

bool SysTickDevice::cancelWait_EventLoopCtx()
{
    SysTickGuard guard;

    if(_state != State::Active)
    {
        return false; // wait not cancelled
    }
    else
    {
        SysTickDisable();
        _state = State::Stopped;
        return true; // wait cancelled
    }
}

bool SysTickDevice::suspendWait_EventLoopCtx()
{
    SysTickGuard guard;

    if(_state != State::Active)
    {
        return false; // wait not suspended
    }
    else
    {
        SysTickDisable();
        _state = State::Suspended;
        return true; // wait suspended
    }
}

void SysTickDevice::resumeWait_EventLoopCtx()
{
    // will be called after suspend, so no critical section needed
    assert(_state == State::Suspended);

    _state = State::Active;
    SysTickEnable();
}

SysTickDevice::WaitUnitTimeType
SysTickDevice::getElapsed_EventLoopCtx() const
{
    // It will be called after cancelWait_EventLoopCtx, so no critical section needed
    assert(_state == State::Stopped);

    const auto elapsedDuration = _waitingDuration - _remainingDuration;
    assert(elapsedDuration >= WaitTimeUnitDuration::zero());

    return elapsedDuration.count();
}

void SysTickDevice::sysTickISR()
{
    assert(_instance);
    _instance->nextTick();
}

} // namespace dev
