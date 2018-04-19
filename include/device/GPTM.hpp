#pragma once

#include <chrono>

#include "util/driverlib/timer.hpp"

#include "device/Peripheral.hpp"

#include "init.hpp"

#include "embxx/util/StaticFunction.h"

namespace device {

class GPTMBase
{
public:
	constexpr static auto Frequency = 80000000; // may be specified by user

	using WaitRep = int;
	using WaitRatio = std::ratio<1, Frequency>;
	using WaitDuration = std::chrono::duration<WaitRep, WaitRatio>;

	template<typename TCallback>
	void setCallback(TCallback&& callback)
	{
		assert(!TimerIsEnabled(_baseAddress, TIMER_A));
		_callback = std::forward<TCallback>(callback);
	}

	void startWait(WaitDuration waitDuration)
	{
		assert(!TimerIsEnabled(_baseAddress, TIMER_A));
		TimerLoadSet(_baseAddress, TIMER_A, waitDuration.count());
		TimerEnable(_baseAddress, TIMER_BOTH);
	}

	std::uint32_t getBaseAddress() const
	{
		return _baseAddress;
	}

protected:
	GPTMBase(std::uint32_t baseAddress, void (*timerISR)())
		:	_baseAddress(baseAddress)
	{
		MAP_TimerConfigure(_baseAddress, TIMER_CFG_ONE_SHOT);

		// // configure prescaler
		// constexpr auto PrescaleValue = (ClockHz / Frequency);
		// static_assert(PrescaleValue * Frequency == ClockHz,
		// 	"Needed Frequency could not be reached without error");
		// MAP_TimerPrescaleSet(_baseAddress, TIMER_A, PrescaleValue);

		// configure interrupts
		TimerIntRegister(_baseAddress, TIMER_A, timerISR);
		TimerIntEnable(_baseAddress, TIMER_TIMA_TIMEOUT);

		// postcondition
		assert(!TimerIsEnabled(_baseAddress, TIMER_A));
	}

	void handleISR()
	{
#ifndef NDEBUG
		const auto maskedIntStatus = TimerMaskedIntStatus(_baseAddress);
		assert(maskedIntStatus == TIMER_TIMA_TIMEOUT);
#endif
		TimerIntClear(_baseAddress, TIMER_TIMA_TIMEOUT);

		assert(_callback);
		_callback();

		// since this timer was in one-shot mode, it should be now disabled
		assert(!TimerIsEnabled(_baseAddress, TIMER_A));
	}

private:
	template<typename T> using StaticFunction = embxx::util::StaticFunction<T>;

	using Callback = StaticFunction<void()>;

	Callback _callback;
	const std::uint32_t _baseAddress;
};

template<std::uint32_t TPeriphID, std::uint32_t TBaseAddress>
class GPTM
	:	public Peripheral<TPeriphID>,
		public GPTMBase
{
public:
	using PeripheralType = Peripheral<TPeriphID>;

	static_assert(TBaseAddress > 0);
	constexpr static auto BaseAddress = TBaseAddress;

	GPTM()
		:	PeripheralType::Peripheral(),
			GPTMBase(BaseAddress, timerISR)
	{
		assert(_instance == nullptr);
		_instance = this;
	}

	~GPTM()
	{
		assert(_instance != nullptr);
		_instance = nullptr;
	}

private:
	static void timerISR()
	{
		assert(_instance);
		_instance->handleISR();
	}

	static GPTM* _instance;
};

using GPTM0 = GPTM<SYSCTL_PERIPH_TIMER0, TIMER0_BASE>;
using GPTM1 = GPTM<SYSCTL_PERIPH_TIMER1, TIMER1_BASE>;
using GPTM2 = GPTM<SYSCTL_PERIPH_TIMER2, TIMER2_BASE>;
using GPTM3 = GPTM<SYSCTL_PERIPH_TIMER3, TIMER3_BASE>;
using GPTM4 = GPTM<SYSCTL_PERIPH_TIMER4, TIMER4_BASE>;
using GPTM5 = GPTM<SYSCTL_PERIPH_TIMER5, TIMER5_BASE>;

} // namespace device
