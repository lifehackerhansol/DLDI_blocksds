# DLDI

Dynamically Linked Device Interface (DLDI) is a storage device interface for Nintendo DS (and GBA) homebrew. You may find more info about DLDI [here](https://www.chishm.com/DLDI/).

This repository hosts both complete and work-in-progress drivers, primarily for Slot-1 devices.

Currently supported devices:

| Driver name | Supported devices                                            |
| ----------- | ------------------------------------------------------------ |
| ak2         | Acekard 2/2.1/2i, r4itt.net 3DS, R4i3D (and probably others) |
| acep        | Ace3DS+, R4 i.L.S., r4isdhc.com.cn, Gateway (Blue)           |
| ez5n        | EZ-Flash Parallel                                            |
| g003        | M3i Zero (GMP-Z003)                                          |
| iply        | iPlayer                                                      |
| m3ds        | M3 DS Real, M3i Zero (other models), iTouchDS, r4rts.com     |
| r4tf        | Original R4, M3 Simply                                       |
| r4idsn      | R4iDSN                                                       |
| rpgs        | Acekard RPG (SD)                                             |
| scds        | SuperCard DSONE                                              |
| scdssdhc    | SuperCard DSONE SDHC                                         |
| sg3d        | Stargate 3DS                                                 |
| ttio        | DSTT                                                         |

## Compiling

0. Install BlocksDS: https://blocksds.github.io/docs/
1. Run `make PLATFORM=(driver name)`

## Adding a new device

> [!TIP]
> You may see the `00example` driver as a reference.

1. Observe the required interface functions in `include/iointerface.h` and implement it to your driver as needed
1. Add device-specific code to `source/platform/(driver name)`
