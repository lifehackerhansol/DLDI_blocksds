// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023
// SPDX-FileContributor: lifehackerhansol, 2023

#include <iointerface.h>
#include <libtwl/card/card.h>
#include <nds/ndstypes.h>

#include "ioa3p.h"

// Initialize the driver. Returns true on success.
bool ioA3P_Startup(void) {
    return true;
}

// Returns true if a card is present and initialized.
bool ioA3P_IsInserted(void) {
    return true;
}

// Reads 512 byte sectors into a buffer that may be unaligned. Returns true on
// success.
bool ioA3P_ReadSectors(uint32_t sector, uint32_t num_sectors, void* buffer) {
    for (int i = 0; i < num_sectors; i++) {
        ioA3PSDReadSector((sector + i), buffer);
        buffer = (u8*)buffer + 0x200;
    }
    return true;
}

// Writes 512 byte sectors from a buffer that may be unaligned. Returns true on
// success.
bool ioA3P_WriteSectors(uint32_t sector, uint32_t num_sectors, const void* buffer) {
    for (int i = 0; i < num_sectors; i++) {
        ioA3PSDWriteSector((sector + i), buffer);
        buffer = (u8*)buffer + 0x200;
    }
    return true;
}

// Clear error flags from the card. Returns true on success.
bool ioA3P_ClearStatus(void) {
    return true;
}

// Shutdowns the card. This may never be called.
bool ioA3P_Shutdown(void) {
    return true;
}

#ifdef PLATFORM_acep

disc_interface_t ioInterface = {.startup = ioA3P_Startup,
                                .is_inserted = ioA3P_IsInserted,
                                .read_sectors = ioA3P_ReadSectors,
                                .write_sectors = ioA3P_WriteSectors,
                                .clear_status = ioA3P_ClearStatus,
                                .shutdown = ioA3P_Shutdown};
#endif
