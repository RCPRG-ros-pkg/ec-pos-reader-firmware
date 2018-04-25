#pragma once

#include "app/encoders/EncoderBase.hpp"

namespace app {
namespace encoders {

struct Encoder0
	:	public EncoderBase<
			SSI0_BASE,
			GPIO_PORTB_BASE, 0,
			GPIO_PORTB_BASE, 1
		>
{

};

} // namespace encoders
} // namespace app
