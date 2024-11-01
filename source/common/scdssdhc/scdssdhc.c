/*
	SuperCard DSONE
	Card IO routines

	Copyright (C) 2023 lifehackerhansol

	SPDX-License-Identifier: Zlib
*/

#include <nds/ndstypes.h>
#include <libtwl/card/card.h>
#include <common/libtwl_ext.h>

#include <common/scdssdhc/scdssdhc.h>

static u32 isSDHC = 0;

static inline u32 SCDSSDHC_SendCommand(const u64 command, u32 latency)
{
	return cardExt_ReadData4Byte(command, SCDSSDHC_CTRL_READ_4B | MCCNT1_LATENCY1(latency));
}

static u32 SCDSSDHC_IsSDHostBusy(void)
{
	return SCDSSDHC_SendCommand(SCDSSDHC_CMD_SD_HOST_BUSY, 0);
}

static void SCDSSDHC_SDSendR0Command(u8 sdio, u32 parameter, u32 latency)
{
	SCDSSDHC_SendCommand(SCDSSDHC_CMD_SD_HOST_PARAM(sdio, parameter, SCDSSDHC_SD_HOST_NORESPONSE), latency);
	while(SCDSSDHC_IsSDHostBusy());
}

static u32 SCDSSDHC_SDSendR1Command(u8 sdio, u32 parameter, u32 latency)
{
	SCDSSDHC_SendCommand(SCDSSDHC_CMD_SD_HOST_PARAM(sdio, parameter, SCDSSDHC_SD_HOST_READ_4B), latency);
	while(SCDSSDHC_IsSDHostBusy());
	return __builtin_bswap32(SCDSSDHC_SendCommand(SCDSSDHC_CMD_SD_HOST_RESPONSE, 0));
}

// TODO: save the response to a buffer (also figure out in which order they're sent)
static void SCDSSDHC_SDSendR2Command(u8 sdio, u32 parameter, u32 latency)
{
	SCDSSDHC_SendCommand(SCDSSDHC_CMD_SD_HOST_PARAM(sdio, parameter, SCDSSDHC_SD_HOST_READ_4B_MULTI), latency);
	while(SCDSSDHC_IsSDHostBusy());

	// TODO: parse this response
	SCDSSDHC_SendCommand(SCDSSDHC_CMD_SD_HOST_RESPONSE, 0);

	for(int i=0; i < 4; i++)
	{
		SCDSSDHC_SendCommand(SCDSSDHC_CMD_SD_HOST_PARAM(sdio, parameter, SCDSSDHC_SD_HOST_NEXT_4B), latency);
		while(SCDSSDHC_IsSDHostBusy());
		// TODO: parse this response
		SCDSSDHC_SendCommand(SCDSSDHC_CMD_SD_HOST_RESPONSE, 0);
	}
	SCDSSDHC_SendCommand(SCDSSDHC_CMD_SD_HOST_PARAM(sdio, parameter, SCDSSDHC_SD_HOST_SEND_STOP_CLK), 0);
	while(SCDSSDHC_IsSDHostBusy());
}

extern void waitByLoop(u32 count);

static void SCDSSDHC_SDSetHostRegister(u8 bits)
{
	SCDSSDHC_SendCommand(SCDSSDHC_CMD_SD_HOST_SET_REGISTER(bits), 0);
	waitByLoop(0x300);
}

void SCDSSDHC_SDGetSDHCStatusFromSRAM(void)
{
	isSDHC = SCDSSDHC_SendCommand(SCDSSDHC_CMD_SRAM_READ_DATA(0x7F9E0), 0) != 0 ? 1 : 0;
}

bool SCDSSDHC_SDInitialize(void)
{
	u32 isSD20 = 0;
	u32 response = 0;

	isSDHC = 0;

	// TODO: What is this command doing?
	cardExt_ReadData(0x6600000000000000ull, 0xA7586000, &response, 1);

	// Reset SD host
	SCDSSDHC_SDSetHostRegister(0);

	// Init
	SCDSSDHC_SDSetHostRegister(SCDSSDHC_SD_HOST_REG_RESET | SCDSSDHC_SD_HOST_REG_400KHZ_CLK | SCDSSDHC_SD_HOST_REG_CLEAN_ROM_MODE);
	SCDSSDHC_SendCommand(SCDSSDHC_CMD_SD_HOST_PARAM(0, 0, SCDSSDHC_SD_HOST_SEND_CLK), SCDSSDHC_CTRL_SD_LOW_CLK_LATENCY);
	waitByLoop(0x1000);

	// CMD0
	SCDSSDHC_SDSendR0Command(0, 0, SCDSSDHC_CTRL_SD_LOW_CLK_LATENCY);
	SCDSSDHC_SendCommand(SCDSSDHC_CMD_SD_HOST_PARAM(0, 0, SCDSSDHC_SD_HOST_SEND_STOP_CLK), SCDSSDHC_CTRL_SD_LOW_CLK_LATENCY);

	// CMD8
	SCDSSDHC_SendCommand(SCDSSDHC_CMD_SD_HOST_PARAM(8, 0x1AA, SCDSSDHC_SD_HOST_READ_4B), SCDSSDHC_CTRL_SD_LOW_CLK_LATENCY);

	u32 retryCount = 9999;
	while(1)
	{
		if(!SCDSSDHC_IsSDHostBusy())
		{
			response = __builtin_bswap32(SCDSSDHC_SendCommand(SCDSSDHC_CMD_SD_HOST_RESPONSE, 0));
			break;
		}
		if (--retryCount == 0)
		{
			SCDSSDHC_SDSetHostRegister(0);
			SCDSSDHC_SDSetHostRegister(SCDSSDHC_SD_HOST_REG_RESET | SCDSSDHC_SD_HOST_REG_400KHZ_CLK | SCDSSDHC_SD_HOST_REG_CLEAN_ROM_MODE);
			response = 0;
			break;
		}
	}

	if(response == 0x1AA)
		isSD20 = 1;

	do
	{
		// CMD55
		SCDSSDHC_SendCommand(SCDSSDHC_CMD_SD_HOST_PARAM(55, 0, SCDSSDHC_SD_HOST_READ_4B), SCDSSDHC_CTRL_SD_LOW_CLK_LATENCY);
		retryCount = 9999;
		while(SCDSSDHC_IsSDHostBusy())
		{
			if (--retryCount == 0)
			{
				SCDSSDHC_SDSetHostRegister(0);
				SCDSSDHC_SDSetHostRegister(SCDSSDHC_SD_HOST_REG_RESET | SCDSSDHC_SD_HOST_REG_400KHZ_CLK | SCDSSDHC_SD_HOST_REG_CLEAN_ROM_MODE);
				return false;
			}
		}
		SCDSSDHC_SendCommand(SCDSSDHC_CMD_SD_HOST_RESPONSE, 0);

		// ACMD41
		u32 parameter = 0x00FC0000;
		if(isSD20)
			parameter |= BIT(30);
		response = SCDSSDHC_SDSendR1Command(41, parameter, SCDSSDHC_CTRL_SD_LOW_CLK_LATENCY);
	} while(!(response & BIT(31)));

	isSDHC = response & BIT(30) ? 1 : 0;

	// CMD2
	SCDSSDHC_SDSendR2Command(2, 0, SCDSSDHC_CTRL_SD_LOW_CLK_LATENCY);

	// CMD3
	response = SCDSSDHC_SDSendR1Command(3, 0, SCDSSDHC_CTRL_SD_LOW_CLK_LATENCY);
	u32 sdio_rca = response & 0xFFFF0000;

	// CMD7
	SCDSSDHC_SDSendR1Command(7, sdio_rca, SCDSSDHC_CTRL_SD_LOW_CLK_LATENCY);

	// ACMD6
	SCDSSDHC_SDSendR1Command(55, sdio_rca, SCDSSDHC_CTRL_SD_LOW_CLK_LATENCY);
	SCDSSDHC_SDSendR1Command(6, 2, SCDSSDHC_CTRL_SD_LOW_CLK_LATENCY);

	// CMD16
	SCDSSDHC_SDSendR1Command(16, 512, SCDSSDHC_CTRL_SD_LOW_CLK_LATENCY);

	SCDSSDHC_SDSetHostRegister(0);
	SCDSSDHC_SDSetHostRegister(SCDSSDHC_SD_HOST_REG_RESET | SCDSSDHC_SD_HOST_REG_CLEAN_ROM_MODE);
	if(isSDHC)
		SCDSSDHC_SDSetHostRegister(SCDSSDHC_SD_HOST_REG_RESET | SCDSSDHC_SD_HOST_REG_CLEAN_ROM_MODE | SCDSSDHC_SD_HOST_REG_SDHC);

	return true;
}

void SCDSSDHC_SDReadSingleSector(u32 sector, void *buffer)
{
	// instruct cart what to read
	SCDSSDHC_SendCommand(SCDSSDHC_CMD_SD_READ_SINGLE_BLOCK(isSDHC ? sector : sector << 9), 0);

	// wait until data ready
	while(SCDSSDHC_SendCommand(SCDSSDHC_CMD_FIFO_BUSY, 0));

	// retrieve data
	cardExt_ReadData(SCDSSDHC_CMD_FIFO_READ_DATA, SCDSSDHC_CTRL_READ_512B, buffer, 128);
}

void SCDSSDHC_SDReadMultiSector(u32 sector, void *buffer, u32 num_sectors)
{
	// instruct cart what to read
	SCDSSDHC_SendCommand(SCDSSDHC_CMD_SD_READ_MULTI_BLOCK(isSDHC ? sector : sector << 9), 0);

	while(1)
	{
		// wait until data ready
		while(SCDSSDHC_SendCommand(SCDSSDHC_CMD_FIFO_BUSY, 0));

		// retrieve data
		cardExt_ReadData(SCDSSDHC_CMD_FIFO_READ_DATA, SCDSSDHC_CTRL_READ_512B, buffer, 128);
		buffer = (u8 *)buffer + 0x200;
		num_sectors--;
		if(num_sectors == 0)
			break;
		SCDSSDHC_SendCommand(SCDSSDHC_CMD_SD_HOST_PARAM(0, 0, SCDSSDHC_SD_HOST_NEXT_DATABLOCK), 0);
	};

	// end read
	SCDSSDHC_SDSendR1Command(12, 0, 0);
}

void SCDSSDHC_SDWriteSingleSector(u32 sector, const void *buffer)
{
	// instruct cart where to write
	SCDSSDHC_SDSendR1Command(24, isSDHC ? sector : sector << 9, 0);

	// write
	cardExt_WriteData(SCDSSDHC_CMD_FIFO_WRITE_DATA, SCDSSDHC_CTRL_WRITE_512B, buffer, 128);
	while(SCDSSDHC_IsSDHostBusy());

	// end write
	SCDSSDHC_SendCommand(SCDSSDHC_CMD_SD_WRITE_END, 0);
	while(SCDSSDHC_IsSDHostBusy());
}

void SCDSSDHC_SDWriteMultiSector(u32 sector, const void *buffer, u32 num_sectors)
{
	// instruct cart where to write
	SCDSSDHC_SDSendR1Command(25, isSDHC ? sector : sector << 9, 0);

	while (num_sectors--) {
		// end write
		// well, it's supposed to be end write. But doing it first is a no-op, and it's also
		// a little simpler to write this way
		SCDSSDHC_SendCommand(SCDSSDHC_CMD_SD_WRITE_END, 0);
		while(SCDSSDHC_IsSDHostBusy());
		// write
		cardExt_WriteData(SCDSSDHC_CMD_FIFO_WRITE_DATA, SCDSSDHC_CTRL_WRITE_512B, buffer, 128);
		while(SCDSSDHC_IsSDHostBusy());
		buffer = (u8 *)buffer + 0x200;
	}

	// *really* end write
	SCDSSDHC_SDSendR1Command(12, 0, 0);
	SCDSSDHC_SendCommand(SCDSSDHC_CMD_SD_WRITE_END, 0);
	while(SCDSSDHC_IsSDHostBusy());
}
