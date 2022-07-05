/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
//#include "ff.h"
#include "ff_gen_drv.h"

/* Not USB in use */
/* Define it in defines.h project file if you want to use USB */
#ifndef FATFS_USE_USB
	#define FATFS_USE_USB				0
#endif

/* Not SDRAM in use */
/* Define it in defines.h project file if you want to use SDRAM */
#ifndef FATFS_USE_SDRAM
	#define FATFS_USE_SDRAM				0
#endif

/* Not SPI FLASH in use */
/* Define it in defines.h project file if you want to use SPI FLASH */
#ifndef FATFS_USE_SPI_FLASH
	#define FATFS_USE_SPI_FLASH			0
#endif

/* Set in defines.h file if you want it */
#ifndef TM_FATFS_CUSTOM_FATTIME
	#define TM_FATFS_CUSTOM_FATTIME		0
#endif

/* Defined in defines.h */
/* We are using FATFS with USB */
#if FATFS_USE_USB == 1 || FATFS_USE_SDRAM == 1 || FATFS_USE_SPI_FLASH == 1
	/* If SDIO is not defined, set to 2, to disable SD card */
	/* You can set FATFS_USE_SDIO in defines.h file */
	/* This is for error fixes */
	#ifndef FATFS_USE_SDIO
		#define FATFS_USE_SDIO			2
	#endif
#else
	/* If USB is not used, set default settings back */
	/* By default, SDIO is used */
	#ifndef FATFS_USE_SDIO
		#define FATFS_USE_SDIO			1
	#endif
#endif

/* USB with FATFS */
#if FATFS_USE_USB == 1
	#include "fatfs_usb.h"
#endif /* FATFS_USE_USB */
/* SDRAM with FATFS */
#if FATFS_USE_SDRAM == 1
	#include "fatfs_sdram.h"
#endif /* FATFS_USE_SDRAM */
/* SPI FLASH with FATFS */
#if FATFS_USE_SPI_FLASH == 1
	#include "fatfs_spi_flash.h"
#endif /* FATFS_USE_SPI_FLASH */

/* Include SD card files if is enabled */
#if FATFS_USE_SDIO == 1
	#include "fatfs_sd_sdio.h"
#elif FATFS_USE_SDIO == 0
	#include "fatfs_sd.h"
#endif


/* Definitions of physical drive number for each media */
#define ATA		   0
#define USB		   1
#define SDRAM      2
#define SPI_FLASH  3

/* Make driver structure */
DISKIO_LowLevelDriver_t FATFS_LowLevelDrivers[_VOLUMES] = {
	{
#if FATFS_USE_SDIO == 1
		TM_FATFS_SD_SDIO_disk_initialize,
		TM_FATFS_SD_SDIO_disk_status,
		TM_FATFS_SD_SDIO_disk_ioctl,
		TM_FATFS_SD_SDIO_disk_write,
		TM_FATFS_SD_SDIO_disk_read
#else
		TM_FATFS_SD_disk_initialize,
		TM_FATFS_SD_disk_status,
		TM_FATFS_SD_disk_ioctl,
		TM_FATFS_SD_disk_write,
		TM_FATFS_SD_disk_read
#endif
	},
//	{
//		TM_FATFS_USB_disk_initialize,
//		TM_FATFS_USB_disk_status,
//		TM_FATFS_USB_disk_ioctl,
//		TM_FATFS_USB_disk_write,
//		TM_FATFS_USB_disk_read
//	},
//	{
//		TM_FATFS_SDRAM_disk_initialize,
//		TM_FATFS_SDRAM_disk_status,
//		TM_FATFS_SDRAM_disk_ioctl,
//		TM_FATFS_SDRAM_disk_write,
//		TM_FATFS_SDRAM_disk_read
//	},
//	{
//		TM_FATFS_SPI_FLASH_disk_initialize,
//		TM_FATFS_SPI_FLASH_disk_status,
//		TM_FATFS_SPI_FLASH_disk_ioctl,
//		TM_FATFS_SPI_FLASH_disk_write,
//		TM_FATFS_SPI_FLASH_disk_read
//	}
};

void TM_FATFS_AddDriver(DISKIO_LowLevelDriver_t* Driver, TM_FATFS_Driver_t DriverName) {
	if (
		DriverName != TM_FATFS_Driver_USER1 &&
		DriverName != TM_FATFS_Driver_USER2
	) {
		/* Return */
		return;
	}
	
	/* Add to structure */
	FATFS_LowLevelDrivers[(uint8_t) DriverName].disk_initialize = Driver->disk_initialize;
	FATFS_LowLevelDrivers[(uint8_t) DriverName].disk_status = Driver->disk_status;
	FATFS_LowLevelDrivers[(uint8_t) DriverName].disk_ioctl = Driver->disk_ioctl;
	FATFS_LowLevelDrivers[(uint8_t) DriverName].disk_read = Driver->disk_read;
	FATFS_LowLevelDrivers[(uint8_t) DriverName].disk_write = Driver->disk_write;
}

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern Disk_drvTypeDef  disk;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Gets Disk Status
  * @param  pdrv: Physical drive number (0..)
  * @retval DSTATUS: Operation status
  */
DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
  DSTATUS stat;

  stat = disk.drv[pdrv]->disk_status(disk.lun[pdrv]);
  return stat;
}

/**
  * @brief  Initializes a Drive
  * @param  pdrv: Physical drive number (0..)
  * @retval DSTATUS: Operation status
  */
DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
  DSTATUS stat = RES_OK;

  if(disk.is_initialized[pdrv] == 0)
  {
    disk.is_initialized[pdrv] = 1;
    stat = disk.drv[pdrv]->disk_initialize(disk.lun[pdrv]);
  }
  return stat;
}

/**
  * @brief  Reads Sector(s)
  * @param  pdrv: Physical drive number (0..)
  * @param  *buff: Data buffer to store read data
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to read (1..128)
  * @retval DRESULT: Operation result
  */
DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	        /* Sector address in LBA */
	UINT count		/* Number of sectors to read */
)
{
  DRESULT res;

  res = disk.drv[pdrv]->disk_read(disk.lun[pdrv], buff, sector, count);
  return res;
}

/**
  * @brief  Writes Sector(s)
  * @param  pdrv: Physical drive number (0..)
  * @param  *buff: Data to be written
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to write (1..128)
  * @retval DRESULT: Operation result
  */
#if _USE_WRITE == 1
DRESULT disk_write (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address in LBA */
	UINT count        	/* Number of sectors to write */
)
{
  DRESULT res;

  res = disk.drv[pdrv]->disk_write(disk.lun[pdrv], buff, sector, count);
  return res;
}
#endif /* _USE_WRITE == 1 */

/**
  * @brief  I/O control operation
  * @param  pdrv: Physical drive number (0..)
  * @param  cmd: Control code
  * @param  *buff: Buffer to send/receive control data
  * @retval DRESULT: Operation result
  */
#if _USE_IOCTL == 1
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
  DRESULT res;

  res = disk.drv[pdrv]->disk_ioctl(disk.lun[pdrv], cmd, buff);
  return res;
}
#endif /* _USE_IOCTL == 1 */

/**
  * @brief  Gets Time from RTC
  * @param  None
  * @retval Time in DWORD
  */
__weak DWORD get_fattime (void)
{
  return 0;
}


/* Function declarations to prevent link errors if functions are not found */
__weak DSTATUS TM_FATFS_SD_SDIO_disk_initialize(void) {return RES_ERROR;}
__weak DSTATUS TM_FATFS_SD_disk_initialize(void) {return RES_ERROR;}
__weak DSTATUS TM_FATFS_USB_disk_initialize(void) {return RES_ERROR;}
__weak DSTATUS TM_FATFS_SDRAM_disk_initialize(void) {return RES_ERROR;}
__weak DSTATUS TM_FATFS_SPI_FLASH_disk_initialize(void) {return RES_ERROR;}

__weak DSTATUS TM_FATFS_SD_SDIO_disk_status(void) {return RES_ERROR;}
__weak DSTATUS TM_FATFS_SD_disk_status(void) {return RES_ERROR;}
__weak DSTATUS TM_FATFS_USB_disk_status(void) {return RES_ERROR;}
__weak DSTATUS TM_FATFS_SDRAM_disk_status(void) {return RES_ERROR;}
__weak DSTATUS TM_FATFS_SPI_FLASH_disk_status(void) {return RES_ERROR;}

__weak DRESULT TM_FATFS_SD_SDIO_disk_ioctl(BYTE cmd, void *buff) {return (DRESULT)STA_NOINIT;}
__weak DRESULT TM_FATFS_SD_disk_ioctl(BYTE cmd, void *buff) {return (DRESULT)STA_NOINIT;}
__weak DRESULT TM_FATFS_USB_disk_ioctl(BYTE cmd, void *buff) {return (DRESULT)STA_NOINIT;}
__weak DRESULT TM_FATFS_SDRAM_disk_ioctl(BYTE cmd, void *buff) {return (DRESULT)STA_NOINIT;}
__weak DRESULT TM_FATFS_SPI_FLASH_disk_ioctl(BYTE cmd, void *buff) {return (DRESULT)STA_NOINIT;}

__weak DRESULT TM_FATFS_SD_SDIO_disk_read(BYTE *buff, DWORD sector, UINT count) {return (DRESULT)STA_NOINIT;}
//added
__weak DRESULT SD_SDIO_ReadBlocks(uint32_t *pData, uint64_t ReadAddr, uint32_t BlockSize, uint32_t NumOfBlocks) {return (DRESULT)STA_NOINIT;}
__weak DRESULT TM_FATFS_SD_disk_read(BYTE *buff, DWORD sector, UINT count) {return (DRESULT)STA_NOINIT;}
__weak DRESULT TM_FATFS_USB_disk_read(BYTE *buff, DWORD sector, UINT count) {return (DRESULT)STA_NOINIT;}
__weak DRESULT TM_FATFS_SDRAM_disk_read(BYTE *buff, DWORD sector, UINT count) {return (DRESULT)STA_NOINIT;}
__weak DRESULT TM_FATFS_SPI_FLASH_disk_read(BYTE *buff, DWORD sector, UINT count) {return (DRESULT)STA_NOINIT;}

__weak DRESULT TM_FATFS_SD_SDIO_disk_write(const BYTE *buff, DWORD sector, UINT count) {return (DRESULT)STA_NOINIT;}
//added
__weak DRESULT SD_SDIO_WriteBlocks(uint32_t *pData, uint64_t WriteAddr, uint32_t BlockSize, uint32_t NumOfBlocks) {return (DRESULT)STA_NOINIT;}
__weak DRESULT TM_FATFS_SD_disk_write(const BYTE *buff, DWORD sector, UINT count) {return (DRESULT)STA_NOINIT;}
__weak DRESULT TM_FATFS_USB_disk_write(const BYTE *buff, DWORD sector, UINT count) {return (DRESULT)STA_NOINIT;}
__weak DRESULT TM_FATFS_SDRAM_disk_write(const BYTE *buff, DWORD sector, UINT count) {return (DRESULT)STA_NOINIT;}
__weak DRESULT TM_FATFS_SPI_FLASH_disk_write(const BYTE *buff, DWORD sector, UINT count) {return (DRESULT)STA_NOINIT;}
