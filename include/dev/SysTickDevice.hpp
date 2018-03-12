#pragma once

#include <chrono>
#include <utility>

#include "embxx/device/context.h"
#include "embxx/error/ErrorStatus.h"
#include "embxx/util/StaticFunction.h"

#include "util/driverlib/systick.hpp"

namespace dev {

class SysTickDevice
{
public:
    //! Tick frequency - how much it ticks in second
    constexpr static int TickFrequency = 1000; // Hz

    using WaitUnitTimeType = int;
    using WaitUnitTimePeriod = std::ratio<1, TickFrequency>;
    using WaitTimeUnitDuration = std::chrono::duration<WaitUnitTimeType, WaitUnitTimePeriod>;

    using EventLoopCtx = embxx::device::context::EventLoop;
    using InterruptCtx = embxx::device::context::Interrupt;

    using ErrorCode = embxx::error::ErrorCode;
    using ErrorStatus = embxx::error::ErrorStatus;
    using WaitCompleteCallback = embxx::util::StaticFunction<void(const ErrorStatus&)>;

    //! Constructor
    SysTickDevice();

    //! Destructor
    ~SysTickDevice();

    //! Sets timer interrupt callback handler
    template <typename TFunc>
    void setWaitCompleteCallback(TFunc&& func);

    //! Starts wait from event loop context
    void startWait(WaitUnitTimeType waitTime, EventLoopCtx);

    //! Starts wait from interrupt context
    void startWait(WaitUnitTimeType waitTime, InterruptCtx);

    //! Cancels wait in event loop context
    bool cancelWait(EventLoopCtx);

    //! Suspends wait in event loop context
    bool suspendWait(EventLoopCtx);

    //! Resumes countdown in event loop context
    void resumeWait(EventLoopCtx);

    //! Returns elapsed time of last executed wait
    WaitUnitTimeType getElapsed(EventLoopCtx) const;

private:
    //! State of the device
    enum class State
    {
    	Stopped, // Not running or cancelled
    	Suspended, // After the call to suspendWait(EventLoopCtx)
    	Active // Currently waiting
    };

    void startWait_BothCtxs(WaitUnitTimeType waitTime);

    bool cancelWait_EventLoopCtx();

    bool suspendWait_EventLoopCtx();

    void resumeWait_EventLoopCtx();

    WaitUnitTimeType getElapsed_EventLoopCtx() const;

    //! Handles next tick of SysTick timer, invoked by sysTickISR
    void nextTick();

	//! Interrupt Service Routine for SysTick
    static void sysTickISR();

    //! Callback to be invoked after ended wait (both successfull and not)
    WaitCompleteCallback _waitCompleteCallback;

    //! Duration to wait, needed in calculation of Elapsed duration
    WaitTimeUnitDuration _waitingDuration = WaitTimeUnitDuration::zero();

    //! Remaining duration to wait, decrements in every tick
    WaitTimeUnitDuration _remainingDuration = WaitTimeUnitDuration::zero();

    //! Current state of the device
    State _state = State::Stopped;

    //! Static instance pointer, needed to handle SysTick interrupts
    static SysTickDevice* _instance;
};

/**
 * @brief Sets timer interrupt callback handler.
 * @details The function is called during the construction of TimerMgr object
 * in non-interrupt context.
 *
 * @param func interrupt callback handler
 */
template <typename TFunc>
inline void
SysTickDevice::setWaitCompleteCallback(TFunc&& func)
{
    _waitCompleteCallback = std::forward<TFunc>(func);
}

/**
 * @brief Starts wait from event loop context
 * @details
 *
 * @param waitTime time to wait
 */
inline void
SysTickDevice::startWait(WaitUnitTimeType waitTime, EventLoopCtx)
{
    startWait_BothCtxs(waitTime);
}

/**
 * @brief Starts wait from interrupt context
 * @details
 *
 * @param waitTime time to wait
 */
inline void
SysTickDevice::startWait(WaitUnitTimeType waitTime, InterruptCtx)
{
    startWait_BothCtxs(waitTime);
}

/**
 * @brief Cancels wait in event loop context
 * @details The function returns true in case the wait was
 * actually cancelled and false when there is no wait
 * in progress.
 *
 * @return Whether wait was actually cancelled or not
 */
inline bool
SysTickDevice::cancelWait(EventLoopCtx)
{
    return cancelWait_EventLoopCtx();
}

/**
 * @brief Suspends wait in event loop context
 * @details The function returns true in case the wait was
 * actually suspended and false when there is no wait in progress.
 * The call to this function will be followed either by
 * resumeWait() or by cancelWait().
 *
 * @return
 */
inline bool
SysTickDevice::suspendWait(EventLoopCtx)
{
    return suspendWait_EventLoopCtx();
}

/**
 * @brief Resumes countdown in event loop context
 * @details
 *
 */
inline void
SysTickDevice::resumeWait(EventLoopCtx)
{
    resumeWait_EventLoopCtx();
}

/**
 * @brief Returns elapsed time of last executed wait
 * @details It will be called right after the cancelWait().
 *
 * @return
 */
inline SysTickDevice::WaitUnitTimeType
SysTickDevice::getElapsed(EventLoopCtx) const
{
    return getElapsed_EventLoopCtx();
}

inline void
SysTickDevice::nextTick()
{
    assert(_state == State::Active);

    assert(_remainingDuration != WaitTimeUnitDuration::zero()); // prevents from negative values
    if(--_remainingDuration == WaitTimeUnitDuration::zero())
    {
    	// wait completed, stop the countdown and invoke the callback
        SysTickDisable();
        _state = State::Stopped;

        const auto errorCode = ErrorCode::Success;
        const auto errorStatus = ErrorStatus(errorCode);
        assert(_waitCompleteCallback);
        _waitCompleteCallback(errorStatus);
    }
}

} // namespace dev
