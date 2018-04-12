#include "device/SysTick.hpp"

#include "init.hpp"

#include <cassert>

namespace device {

SysTickBase* SysTickBase::_instance = nullptr;

class SysTickBaseGuard
{
public:
    SysTickBaseGuard() noexcept
    {
        HWREG(NVIC_ST_CTRL) &= ~(NVIC_ST_CTRL_INTEN); // disable SysTickBase interrupts
        __asm("dsb"); // data synchronization barrier
        __asm("isb"); // instruction synchronization barrier
    }

    ~SysTickBaseGuard() noexcept
    {
        HWREG(NVIC_ST_CTRL) |= NVIC_ST_CTRL_INTEN; // enable SysTickBase interrupts
    }
};

/**
 * @brief Constructor
 * @details
 */
SysTickBase::SysTickBase()
{
	assert(_instance == nullptr);
	_instance = this;

	SysTickIntRegister(sysTickISR);
    // SysTickBase interrupts is enabled after call to SysTickIntRegister

	constexpr std::uint32_t period = (ClockHz / TickFrequency);
	SysTickPeriodSet(period);
}

/**
 * @brief Destructor
 * @details
 */
SysTickBase::~SysTickBase()
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

void SysTickBase::startWait_BothCtxs(WaitUnitTimeType waitTime)
{
    // Before startWait_EventLoopCtx, cancelWait_EventLoopCtx will be called
    // so no critical section needed. This same in interrupt context.
    assert(_state == State::Stopped);

    _waitingDuration = _remainingDuration = WaitTimeUnitDuration(waitTime);
    _state = State::Active;
    SysTickClear();
    SysTickEnable();
}

bool SysTickBase::cancelWait_EventLoopCtx()
{
    SysTickBaseGuard guard;

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

bool SysTickBase::suspendWait_EventLoopCtx()
{
    SysTickBaseGuard guard;

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

void SysTickBase::resumeWait_EventLoopCtx()
{
    // will be called after suspend, so no critical section needed
    assert(_state == State::Suspended);

    _state = State::Active;
    SysTickEnable();
}

SysTickBase::WaitUnitTimeType
SysTickBase::getElapsed_EventLoopCtx() const
{
    // It will be called after cancelWait_EventLoopCtx, so no critical section needed
    assert(_state == State::Stopped);

    const auto elapsedDuration = _waitingDuration - _remainingDuration;
    assert(elapsedDuration >= WaitTimeUnitDuration::zero());

    return elapsedDuration.count();
}

void SysTickBase::sysTickISR()
{
    assert(_instance);
    _instance->nextTick();
}

} // namespace device
