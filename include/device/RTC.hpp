#pragma once

#include <chrono>
#include <limits>
#include <ratio>
#include <cstdint>
#include "tivaware/driverlib/hibernate.h"

namespace device {

class RTC
{
public:
	using SecondsRep = int;
	using SecondsRatio = std::ratio<1>;
	using Seconds = std::chrono::duration<SecondsRep, SecondsRatio>;

	using SubSecondsRep = int;
	using SubSecondsRatio = std::nano;
	using SubSeconds = std::chrono::duration<SubSecondsRep, SubSecondsRatio>;

	//! Constructor
	RTC();

	//! Destructor
	~RTC();

	Seconds getSeconds() const;

	SubSeconds getSubSeconds() const;

private:

};

inline RTC::Seconds
RTC::getSeconds() const
{
	return Seconds(HibernateRTCGet());
}

inline RTC::SubSeconds
RTC::getSubSeconds() const
{
	using RTCSubSecondsRep = std::uint32_t;
	using RTCSubSecondsRatio = std::ratio<1, 32768>;
	using RTCSubSeconds = std::chrono::duration<RTCSubSecondsRep, RTCSubSecondsRatio>;

	const auto rtcSubSeconds = RTCSubSeconds(HibernateRTCSSGet());

	const auto subSeconds =
		std::chrono::duration_cast<SubSeconds>(rtcSubSeconds);

	return subSeconds;
}

} // namespace device
