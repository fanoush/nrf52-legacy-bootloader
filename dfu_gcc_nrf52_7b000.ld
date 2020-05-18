/* Linker script to configure memory regions. */

SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)

MEMORY
{
  /** Flash start address for the bootloader. This setting will also be stored in UICR to allow the
   *  MBR to init the bootloader when starting the system. This value must correspond to 
   *  BOOTLOADER_REGION_START found in dfu_types.h. The system is prevented from starting up if 
   *  those values do not match. The check is performed in main.c, see
   *  APP_ERROR_CHECK_BOOL(*((uint32_t *)NRF_UICR_BOOT_START_ADDRESS) == BOOTLOADER_REGION_START);
   */
  FLASH (rx) : ORIGIN = 0x7B000, LENGTH = 0x7E000-0x7B000 /* needs to fit below MBR_PARAMS */

  /** RAM Region for bootloader. This setting is suitable when used with s132. */
  RAM (rwx) :  ORIGIN = 0x20002C00, LENGTH = 0x5380

  /** Location of non initialized RAM. Non initialized RAM is used for exchanging bond information
   *  from application to bootloader when using buttonluss DFU OTA. 
   */
  NOINIT (rwx) :  ORIGIN = 0x20007F80, LENGTH = 0x80

  /** Location of bootloader setting in flash. */
  BOOTLOADER_SETTINGS (rw) : ORIGIN = 0x0007F000, LENGTH = 0x1000

  /** Location in UICR where bootloader start address is stored. */
  UICR_BOOTLOADER (r) : ORIGIN = 0x10001014, LENGTH = 0x04
  
  /** Location of mbr params page in flash. */
  MBR_PARAMS_PAGE (rw) : ORIGIN = 0x0007E000, LENGTH = 0x1000
  
  /** Location in UICR where mbr params page address is stored. */
  UICR_MBR_PARAM_PAGE(r) : ORIGIN = 0x10001018, LENGTH = 0x04
}

INCLUDE "dfu_gcc_nrf52.ld"
