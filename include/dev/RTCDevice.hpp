#pragma once

#include <cstdint>
#include "tivaware/driverlib/hibernate.h"

namespace dev {

class RTCDevice
{
public:
	//! Constructor
	RTCDevice();

	//! Destructor
	~RTCDevice();

	int getSeconds() const;

	int getSubSeconds() const;

private:

};

inline int
RTCDevice::getSeconds() const
{
	return HibernateRTCGet();
}

inline int
RTCDevice::getSubSeconds() const
{
	return HibernateRTCSSGet();
}

} // namespace dev
