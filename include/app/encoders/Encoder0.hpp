#pragma once

#include "app/encoders/EncoderBase.hpp"

namespace app {
namespace encoders {

struct Encoder0
	:	public EncoderBase<
			SSI0_BASE, SYSCTL_PERIPH_SSI0, INT_SSI0,
			GPIO_PORTB_BASE, 0,
			GPIO_PORTB_BASE, 1
		>
{
	using EncoderBaseType = EncoderBase<
		SSI0_BASE, SYSCTL_PERIPH_SSI0, INT_SSI0,
		GPIO_PORTB_BASE, 0,
		GPIO_PORTB_BASE, 1
	>;

	using EncoderBaseType::EncoderBase;
};

} // namespace encoders
} // namespace app
