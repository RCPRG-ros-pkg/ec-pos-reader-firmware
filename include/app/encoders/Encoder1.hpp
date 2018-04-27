#pragma once

#include "app/encoders/EncoderBase.hpp"

namespace app {
namespace encoders {

struct Encoder1
	:	public EncoderBase<
			SSI2_BASE, INT_SSI2,
			GPIO_PORTB_BASE, 2,
			GPIO_PORTB_BASE, 3
		>
{
	using EncoderBaseType = EncoderBase<
		SSI2_BASE, INT_SSI2,
		GPIO_PORTB_BASE, 2,
		GPIO_PORTB_BASE, 3
	>;

	// Forward base constructor
	using EncoderBaseType::EncoderBase;
};

} // namespace encoders
} // namespace app
