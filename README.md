# nrf52-legacy-bootloader

SDK11 Nordic legacy DFU OTA bootloader

### Getting started

This needs slightly patched Nordic SDK11, run get-sdk11.sh and hopefully it should be downloaded, extracted into SDK11 folder and patched with sdk11.patch. If it fails then do it by hand :-) Needs `wget`, `unzip`, `patch` and `dos2unix` to convert text files to unix format.

Then just type `make`, output is in `_build/nrf52832_xxaa_s132_<address>.hex`.

### Changes

You can change some variables in Makefile

- BOOTLOADER_START - bootloader start address, one of `78000`, `7a000` or `7b000`
- -DFEED_WATCHDOG - will keep feeding watchdog if it is enabled at bootloader entry
- -DDISABLE_BUTTONLESS_DFU - trims down size, disables Buttonless DFU
- -DDISABLE_DFU_APPCHECK - trims down size, disables check for application and device version in dfu package, softdevice version is still checked
- -DSMALL_INTERRUPT_VECTORS - trims down size, make initial interrupt vectors area size smaller (from 0x400 to 0x100)
