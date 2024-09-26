# DLDI

Dynamically Linked Device Interface (DLDI) is a storage device interface for Nintendo DS (and GBA) homebrew. You may find more info about DLDI [here](https://www.chishm.com/DLDI/).

This repository hosts both complete and work-in-progress drivers, primarily for Slot-1 devices.

Currently supported devices:

| Driver name | Supported devices                                        |
| ----------- | -------------------------------------------------------- |
| acep        | Ace3DS+, R4 i.L.S., r4isdhc.com.cn, Gateway (Blue)       |
| g003        | M3i Zero (GMP-Z003)                                      |
| m3ds        | M3 DS Real, M3i Zero (other models), iTouchDS, r4rts.com |
| r4tf        | Original R4, M3 Simply                                   |
| sg3d        | Stargate 3DS                                             |

## Compiling

0. Install BlocksDS: https://blocksds.github.io/docs/
1. Run `make PLATFORM=(driver name)`
