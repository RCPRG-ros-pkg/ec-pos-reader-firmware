/**
 * ssi.hpp
 * Utilities, extending TI Driverlib library SSI module.
 * Adds funtions not present in SSI module, like SSIRxEmpty etc
 * Author: akowalew
 */

#pragma once

#include <cstdint>
#include <cassert>
#include "tivaware/inc/hw_types.h"
#include "tivaware/inc/hw_memmap.h"
#include "tivaware/inc/hw_ssi.h"
#include "tivaware/driverlib/ssi.h"

//! Specifies SSI serial clock polarity in idle mode. Used by SSIPolaritySet
enum class SSIPolarity
{
	SteadyLow, //! SSInCLK will be steady LOW when idle
	SteadyHigh //! SSInCLK will be steady HIGH when idle
};

//! Specifies SSI mode. Used by SSIGetMode
enum class SSIMode
{
	Master = SSI_MODE_MASTER,
	Slave = SSI_MODE_SLAVE
};

std::uint32_t SSIGetDataNow(std::uint32_t baseAddress);
bool SSIEnabled(std::uint32_t baseAddress);
void SSIEOTEnable(std::uint32_t baseAddress);
void SSIEOTDisable(std::uint32_t baseAddress);
int SSIGetBitRate(std::uint32_t baseAddress);
std::size_t SSIGetDataWidth(std::uint32_t baseAddress);
SSIMode SSIGetMode(std::uint32_t baseAddress);
bool SSIIdle(std::uint32_t baseAddress);
std::uint32_t SSIMaskedIntStatus(std::uint32_t baseAddress);
void SSIPolaritySet(std::uint32_t baseAddress, SSIPolarity ssiPolarity);
void SSIPutDataNow(std::uint32_t baseAddress, std::uint32_t data);
std::uint32_t SSIRawIntStatus(std::uint32_t baseAddress);
bool SSIRxEmpty(std::uint32_t baseAddress);
bool SSIRxNotEmpty(std::uint32_t baseAddress);
bool SSITxFull(std::uint32_t baseAddress);
bool SSITxNotFull(std::uint32_t baseAddress);

/**
 * @brief Force gets data from SSI receiver FIFO
 * @details Precondition: SSI receive FIFO in NOT empty
 *
 * @param baseAddress base address of SSI module
 */
inline std::uint32_t
SSIGetDataNow(std::uint32_t baseAddress)
{
	assert(SSIRxNotEmpty(SSI0_BASE));
	return (HWREG(baseAddress + SSI_O_DR));
}

/**
 * @brief Checks, whether SSI module is enabled or not
 * @details
 *
 * @param baseAddress base address of SSI module
 */
inline bool
SSIEnabled(std::uint32_t baseAddress)
{
	return (HWREG(baseAddress + SSI_O_CR1) |= SSI_CR1_SSE);
}

/**
 * @brief Enables End Of Transmission for SSI.
 * @details After that, TXRIS bit in SSIRIS register signals, whether
 * transmission ended, or not
 * Precondition: Master mode must be enabled
 * @param baseAddress base address of SSI module
 */
inline void
SSIEOTEnable(std::uint32_t baseAddress)
{
	const auto ssicr1 = HWREG(baseAddress + SSI_O_CR1);
	assert(!(ssicr1 & SSI_CR1_MS)); // master mode must be enabled
	HWREG(baseAddress + SSI_O_CR1) = (ssicr1 | SSI_CR1_EOT);
}

/**
 * @brief Disables End Of Transmission for SSI.
 * @details After that, TXRIS bit in SSIRIS register signals, whether
 * SSI transmit FIFO is half full or less
 *
 * @param baseAddress base address of SSI module
 */
inline void
SSIEOTDisable(std::uint32_t baseAddress)
{
	HWREG(baseAddress + SSI_O_CR1) &= (~SSI_CR1_EOT);
}

/**
 * @brief Gets data width, with which SSI is working
 * @details
 * @param baseAddress base address of SSI module
 */
inline std::size_t
SSIGetDataWidth(std::uint32_t baseAddress)
{
	const auto dss = (HWREG(baseAddress + SSI_O_CR0) & SSI_CR0_DSS_M);
	assert(dss >= 3 && dss <= 15);

	const auto dataWidth = (dss + 1);
	return dataWidth;
}

/**
 * @brief Gets bit rate of SSI module
 * @details TODO
 *
 * @param baseAddress base address of SSI module
 */
inline int
SSIGetBitRate(std::uint32_t baseAddress)
{
	static_cast<void>(baseAddress);
	assert(false); // not implemented
	return 0;
}

/**
 * @brief Gets mode, at which SSI module is working
 * @details
 *
 * @param baseAddress base address of SSI module
 */
inline SSIMode
SSIGetMode(std::uint32_t baseAddress)
{
	return (HWREG(baseAddress + SSI_O_CR1) & SSI_CR1_MS)
		? SSIMode::Slave : SSIMode::Master;
}

/**
 * @brief Checks, if SSI module is idle
 * @details Under the hood, it checks BSY flag in SSI Status Register
 *
 * @param baseAddress base address of SSI module
 */
inline bool
SSIIdle(std::uint32_t baseAddress)
{
	return !(HWREG(baseAddress + SSI_O_SR) & SSI_SR_BSY);
}

/**
 * @brief Returns SSI Masked Interrupt Status
 * @details
 *
 * @param baseAddress base address of SSI module
 */
inline std::uint32_t
SSIMaskedIntStatus(std::uint32_t baseAddress)
{
	return (HWREG(baseAddress + SSI_O_MIS));
}

/**
 * @brief Sets polarity of SSI serial clock.
 * @details Configures behaviour of SSInCLK pin when data is not transferred.
 *  If ssiPolarity is SteadyHigh, SSInCLK will be logical HIGH in Idle state,
 *  else LOW
 *
 * @param baseAddress base address of SSI module
 * @param ssiPolarity polarity of SSI serial clock
 */
inline void
SSIPolaritySet(std::uint32_t baseAddress, SSIPolarity ssiPolarity)
{
	const auto ssicr0 = HWREG(baseAddress + SSI_O_CR0);
	HWREG(baseAddress + SSI_O_CR0) =
		(ssiPolarity == SSIPolarity::SteadyHigh) ?
			(ssicr0 | SSI_CR0_SPO) : (ssicr0 & (~SSI_CR0_SPO));
}

/**
 * @brief Force puts data into SSI transmit FIFO
 * @details Precondition: SSI transmit FIFO is NOT full
 *
 * @param baseAddress base address of SSI module
 * @param data data to send
 */
inline void
SSIPutDataNow(std::uint32_t baseAddress, std::uint32_t data)
{
	assert(SSITxNotFull(baseAddress));
	HWREG(baseAddress + SSI_O_DR) = data;
}

/**
 * @brief Returns SSI Raw Interrupt Status
 * @details
 *
 * @param baseAddress base address of SSI module
 */
inline std::uint32_t
SSIRawIntStatus(std::uint32_t baseAddress)
{
	return (HWREG(baseAddress + SSI_O_RIS));
}

/**
 * @brief Checks, if SSI receiver FIFO is empty
 *
 * @param baseAddress base address of SSI module
 */
inline bool
SSIRxEmpty(std::uint32_t baseAddress)
{
	return !(SSIRxNotEmpty(baseAddress));
}

/**
 * @brief Checks, if SSI receiver FIFO is NOT empty
 *
 * @param baseAddress base address of SSI module
 */
inline bool
SSIRxNotEmpty(std::uint32_t baseAddress)
{
	return (HWREG(baseAddress + SSI_O_SR) & SSI_SR_RNE);
}

/**
 * @brief Checks, if SSI transmitter FIFO is full
 *
 * @param baseAddress base address of SSI module
 */
inline bool
SSITxFull(std::uint32_t baseAddress)
{
	return !(SSITxNotFull(baseAddress));
}

/**
 * @brief Checks, if SSI transmitter FIFO is NOT full
 *
 * @param baseAddress base address of SSI module
 */
inline bool
SSITxNotFull(std::uint32_t baseAddress)
{
	return (HWREG(baseAddress + SSI_O_SR) & SSI_SR_TNF);
}
