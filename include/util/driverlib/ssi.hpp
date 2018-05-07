/**
 * ssi.hpp
 * Utilities, extending TI Driverlib library SSI module.
 * Adds funtions not present in SSI module, like SSIRxEmpty etc
 * Author: akowalew
 */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include "tivaware/inc/hw_types.h"
#include "tivaware/inc/hw_memmap.h"
#include "tivaware/inc/hw_ssi.h"
#include "tivaware/driverlib/ssi.h"

#define SSI_POL_STEADY_LOW 0
#define SSI_POL_STEADY_HIGH 1

#define SSI_MIN_DATA_WIDTH 4
#define SSI_MAX_DATA_WIDTH 16

#define SSI_FIFO_SIZE 8

#define SSI_INT_FLAGS_ALL (SSI_TXFF | SSI_RXFF | SSI_RXTO | SSI_RXOR)

typedef uint32_t SSIDataType;

uint32_t SSIDataGetNow(uint32_t baseAddress);
bool SSIIsEnabled(uint32_t baseAddress);
uint32_t SSIIntEnabledGet(uint32_t baseAddress);
void SSIEOTEnable(uint32_t baseAddress);
void SSIEOTDisable(uint32_t baseAddress);
uint32_t SSIBitRateGet(uint32_t baseAddress, uint32_t clockRate);
void SSIBitRateSet(uint32_t baseAddress, uint32_t clockRate, uint32_t bitRate);
size_t SSIDataWidthGet(uint32_t baseAddress);
void SSIDataWidthSet(uint32_t baseAddress, size_t dataWidth);
uint32_t SSIModeGet(uint32_t baseAddress);
bool SSIIdle(uint32_t baseAddress);
uint32_t SSIMaskedIntStatus(uint32_t baseAddress);
void SSIPolaritySet(uint32_t baseAddress, uint32_t polarity);
void SSIDataPutNow(uint32_t baseAddress, SSIDataType data);
uint32_t SSIRawIntStatus(uint32_t baseAddress);
bool SSIRxFull(uint32_t baseAddress);
bool SSIRxNotFull(uint32_t baseAddress);
bool SSIRxEmpty(uint32_t baseAddress);
bool SSIRxNotEmpty(uint32_t baseAddress);
bool SSITxFull(uint32_t baseAddress);
bool SSITxNotFull(uint32_t baseAddress);
bool SSITxEmpty(uint32_t baseAddress);
bool SSITxNotEmpty(uint32_t baseAddress);

/**
 * @brief Force gets data from SSI receiver FIFO
 * @details Precondition: SSI receive FIFO in NOT empty
 *
 * @param baseAddress base address of SSI module
 */
inline uint32_t
SSIDataGetNow(uint32_t baseAddress)
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
SSIIsEnabled(uint32_t baseAddress)
{
	return (HWREG(baseAddress + SSI_O_CR1) & SSI_CR1_SSE);
}

/**
 * @brief Returns enabled SSI interrupts
 * @details [long description]
 *
 * @param baseAddress [description]
 */
inline uint32_t
SSIIntEnabledGet(uint32_t baseAddress)
{
    return HWREG(baseAddress + SSI_O_IM);
}

/**
 * @brief Enables End Of Transmission for SSI.
 * @details After that, TXRIS bit in SSIRIS register signals, whether
 * transmission ended, or not
 * Precondition: Master mode must be enabled
 * @param baseAddress base address of SSI module
 */
inline void
SSIEOTEnable(uint32_t baseAddress)
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
SSIEOTDisable(uint32_t baseAddress)
{
	HWREG(baseAddress + SSI_O_CR1) &= (~SSI_CR1_EOT);
}

/**
 * @brief Sets bit rate of SSI transmission
 * @details [long description]
 *
 * @param baseAddress [description]
 * @param clockRate [description]
 * @param bitRate [description]
 */
inline void
SSIBitRateSet(uint32_t baseAddress, uint32_t clockRate, uint32_t bitRate)
{
	static_cast<void>(baseAddress);
	static_cast<void>(clockRate);
	static_cast<void>(bitRate);
	assert(!"SSIBitRateSet: not implemented");
}

/**
 * @brief Gets data width, with which SSI is working
 * @details
 * @param baseAddress base address of SSI module
 */
inline size_t
SSIDataWidthGet(uint32_t baseAddress)
{
	const auto dss = (HWREG(baseAddress + SSI_O_CR0) & SSI_CR0_DSS_M);
	assert(dss >= 3 && dss <= 15);

	const auto dataWidth = (dss + 1);
	return dataWidth;
}

/**
 * @brief Sets data width, with which SSI will work
 * @details [long description]
 *
 * @param baseAddress [description]
 * @param dataWidth [description]
 */
inline void
SSIDataWidthSet(uint32_t baseAddress, size_t dataWidth)
{
	assert(dataWidth >= 4 && dataWidth <= 16);

	// get current value of SSICR0 register
	uint32_t cr0 = HWREG(baseAddress + SSI_O_CR0);

	// clear DSS bitfield
	cr0 &= ~SSI_CR0_DSS_M;

	// write new DSS bitfield
	cr0 |= (dataWidth - 1);

	// write new value to SSICR0 register
	HWREG(baseAddress + SSI_O_CR0) |= cr0;
}

/**
 * @brief Gets mode, at which SSI module is working
 * @details
 *
 * @param baseAddress base address of SSI module
 */
inline uint32_t
SSIModeGet(uint32_t baseAddress)
{
	return (HWREG(baseAddress + SSI_O_CR1) & SSI_CR1_MS)
		? SSI_MODE_SLAVE : SSI_MODE_MASTER;
}

/**
 * @brief Checks, if SSI module is idle
 * @details Under the hood, it checks BSY flag in SSI Status Register
 *
 * @param baseAddress base address of SSI module
 */
inline bool
SSIIdle(uint32_t baseAddress)
{
	return !(HWREG(baseAddress + SSI_O_SR) & SSI_SR_BSY);
}

/**
 * @brief Returns SSI Masked Interrupt Status
 * @details
 *
 * @param baseAddress base address of SSI module
 */
inline uint32_t
SSIMaskedIntStatus(uint32_t baseAddress)
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
SSIPolaritySet(uint32_t baseAddress, uint32_t polarity)
{
	const auto ssicr0 = HWREG(baseAddress + SSI_O_CR0);
	HWREG(baseAddress + SSI_O_CR0) =
		(polarity == SSI_POL_STEADY_HIGH) ?
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
SSIDataPutNow(uint32_t baseAddress, SSIDataType data)
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
inline uint32_t
SSIRawIntStatus(uint32_t baseAddress)
{
	return (HWREG(baseAddress + SSI_O_RIS));
}

/**
 * @brief Checks, if SSI receiver FIFO is empty
 *
 * @param baseAddress base address of SSI module
 */
inline bool
SSIRxEmpty(uint32_t baseAddress)
{
	return !(SSIRxNotEmpty(baseAddress));
}

/**
 * @brief Checks, if SSI receiver FIFO is NOT empty
 *
 * @param baseAddress base address of SSI module
 */
inline bool
SSIRxNotEmpty(uint32_t baseAddress)
{
	return (HWREG(baseAddress + SSI_O_SR) & SSI_SR_RNE);
}

/**
 * @brief Checks, if SSI transmitter FIFO is full
 *
 * @param baseAddress base address of SSI module
 */
inline bool
SSITxFull(uint32_t baseAddress)
{
	return (!SSITxNotFull(baseAddress));
}

/**
 * @brief Checks, if SSI transmitter FIFO is NOT full
 *
 * @param baseAddress base address of SSI module
 */
inline bool
SSITxNotFull(uint32_t baseAddress)
{
	return (HWREG(baseAddress + SSI_O_SR) & SSI_SR_TNF);
}

inline bool
SSIRxFull(uint32_t baseAddress)
{
	return (HWREG(baseAddress + SSI_O_SR) & SSI_SR_RFF);
}

inline bool
SSIRxNotFull(uint32_t baseAddress)
{
	return (!SSIRxFull(baseAddress));
}

inline bool
SSITxEmpty(uint32_t baseAddress)
{
	return (HWREG(baseAddress + SSI_O_SR) & SSI_SR_TFE);

}

inline bool
SSITxNotEmpty(uint32_t baseAddress)
{
	return (!SSITxEmpty(baseAddress));
}
