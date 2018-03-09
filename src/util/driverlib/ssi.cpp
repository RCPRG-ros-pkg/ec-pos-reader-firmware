#include "util/driverlib/ssi.hpp"

/**
 * @brief Gets bit rate of SSI module
 * @details
 *
 * @param baseAddress base address of SSI module
 * @param clockRate clock speed for SSI module (uint32 for compability)
 */
std::uint32_t SSIGetBitRate(std::uint32_t baseAddress, std::uint32_t clockRate)
{
	assert(clockRate != 0);

	int serialClockRate =
		((HWREG(baseAddress + SSI_O_CR0) & SSI_CR0_SCR_M) >> SSI_CR0_SCR_S);
	assert(serialClockRate >= 0 && serialClockRate <= 255); // datasheet

	int clockPrescalerDivisor =
		((HWREG(baseAddress + SSI_O_CPSR) & SSI_CPSR_CPSDVSR_M) >> SSI_CPSR_CPSDVSR_S);
	assert(clockPrescalerDivisor >= 2 && clockPrescalerDivisor <= 254); // datasheet
	assert((clockPrescalerDivisor % 2) == 0); // datasheet

	const int maxBitRate = clockPrescalerDivisor * (serialClockRate + 1) + 1;
	assert(maxBitRate != 0);

	const int bitRate = clockRate / maxBitRate;
	assert(bitRate > 0);

	return bitRate;
}
