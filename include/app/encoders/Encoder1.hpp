#pragma once

#include "app/encoders/EncoderBase.hpp"

namespace app {
namespace encoders {

struct Encoder1
	:	public EncoderBase<
			SSI2_BASE,
			GPIO_PORTB_BASE, 2,
			GPIO_PORTB_BASE, 3
		>
{

};

} // namespace encoders
} // namespace app
