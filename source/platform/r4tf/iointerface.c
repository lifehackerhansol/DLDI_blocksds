// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023
// SPDX-FileContributor: lifehackerhansol, 2023

#include <nds/ndstypes.h>
#include <libtwl/card/card.h>
#include <iointerface.h>

#include "ior4.h"

// Initialize the driver. Returns true on success.
bool ioR4_Startup(void)
{
	// Confirm card is actually responding and is actually an R4
	return (ioR4SendCommand(IOR4_CMD_CARD_INFO) & 7) == 4;
}

// Returns true if a card is present and initialized.
bool ioR4_IsInserted(void)
{
	// Confirm card is actually responding and is actually an R4
	return (ioR4SendCommand(IOR4_CMD_CARD_INFO) & 7) == 4;
}

// Reads 512 byte sectors into a buffer that may be unaligned. Returns true on
// success.
bool ioR4_ReadSectors(uint32_t sector, uint32_t num_sectors, void *buffer)
{
	for (int i = 0; i < num_sectors; i++)
	{
		ioR4SDReadSector((sector + i) << 9, buffer);
		buffer = (u8 *)buffer + 0x200;
	}
	return true;
}

// Writes 512 byte sectors from a buffer that may be unaligned. Returns true on
// success.
bool ioR4_WriteSectors(uint32_t sector, uint32_t num_sectors, const void *buffer)
{
	for (int i = 0; i < num_sectors; i++)
	{
		ioR4SDWriteSector((sector + i) << 9, buffer);
		buffer = (u8 *)buffer + 0x200;
	}
	return true;
}

// Clear error flags from the card. Returns true on success.
bool ioR4_ClearStatus(void)
{
	return true;
}

// Shutdowns the card. This may never be called.
bool ioR4_Shutdown(void)
{
	return true;
}

#ifdef PLATFORM_r4tf

disc_interface_t ioInterface = {
	.startup = ioR4_Startup,
	.is_inserted = ioR4_IsInserted,
	.read_sectors = ioR4_ReadSectors,
	.write_sectors = ioR4_WriteSectors,
	.clear_status = ioR4_ClearStatus,
	.shutdown = ioR4_Shutdown
};

#endif
