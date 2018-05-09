#pragma once

#include "app/encoders/EncoderBase.hpp"

namespace app {
namespace encoders {

struct Encoder1
	:	public EncoderBase<
			SSI2_BASE, SYSCTL_PERIPH_SSI2, INT_SSI2
		>
{
	using EncoderBaseType = EncoderBase<
		SSI2_BASE, SYSCTL_PERIPH_SSI2, INT_SSI2
	>;

	// Forward base constructor
	using EncoderBaseType::EncoderBase;
};

} // namespace encoders
} // namespace app
