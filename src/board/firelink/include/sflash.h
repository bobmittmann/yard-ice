/* 
 * Copyright(C) 2015 Robinson Mittmann. All Rights Reserved.
 *
 * This file is part of the YARD-ICE.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You can receive a copy of the GNU Lesser General Public License from
 * http://www.gnu.org/
 */

/**
 * @file sflash.h
 * @brief Simple serial flash driver
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */

#ifndef __SFLASH_H__
#define __SFLASH_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/* Serial flash device driver (opaque structure) */
struct sflash_dev;

/* Serial flash chip information structure */
struct sflash_inf {
	uint8_t manufacturer;
	uint8_t device_type;
	uint8_t capacity;
	uint32_t sector_size;
	uint16_t sector_count;
	uint16_t subsector_size;
	uint16_t page_size;
};

#ifdef __cplusplus
extern "C" {
#endif

/* Initialize the serial flash driver and returns an
 * instance for subsequent operations. */
struct sflash_dev * sflash_init(void);

/** @brief Probe the SPI bus for a flash device.
 *
 * fill in the flash information structure.
 *
 * @param sf The serial flash device.
 * @param inf The serial flash information structure.
 * @return @b -1 if no serial flash is found, 0 otherwise.
 */
int sflash_probe(struct sflash_dev * sf, struct sflash_inf * inf);

/* ------------------------------------------------------------------------
   Serial Flash Public API (Low Level)
   ------------------------------------------------------------------------- */

/** @defgroup lowlevel Serial Flash Low Level API
 * This functions provide a basic operations for page read/write and
 * block/sector erasing.
 * @{
 */

/** @brief Erase a single sector.
 *
 * A flash sector is a block of 64KiB.
 *
 * @param sf The serial flash device.
 * @param addr The sector address.
 * @return On error @b -1 is returned, 0 otherwise.
 */
int sflash_sector_erase(struct sflash_dev * sf, uint32_t addr);

/** @brief Erase a single subsector.
 *
 * A flash subsector is a sector of 4KiB.
 *
 * @param sf The serial flash device.
 * @param addr The subsector address.
 * @return On error @b -1 is returned, 0 otherwise.
 */
int sflash_subsector_erase(struct sflash_dev * sf, uint32_t addr);

/** @brief Write data into a page.
 *
 * This function writes up to a full page starting at address @p addr.
 *
 * @param sf The serial flash device.
 * @param addr The page address.
 * @param buf A pointer to the data to be written.
 * @param count Number of bytes in the buffer.
 * @return On success, the number of bytes written is returned.
 * On error @b -1 is returned.
 */
int sflash_page_write(struct sflash_dev * sf, uint32_t addr, 
					  const void * buf, size_t count);

/** @brief Read data from a page.
 *
 * This function attempts to read up to a @p count bytes starting at
 * address @p addr. This call returns at most a full flash page.
 *
 * @param sf The serial flash device.
 * @param addr The page address.
 * @param buf A pointer to the buffer to receive the data.
 * @param count Maximum number of bytes to read from the flash.
 * @return On success, the number of bytes read is returned.
 * On error @b -1 is returned.
 */
int sflash_page_read(struct sflash_dev * sf, uint32_t addr, 
					 void * buf, size_t count);

/**@}*/

/* ------------------------------------------------------------------------
   Serial Flash Public API (High Level)
   ------------------------------------------------------------------------- */

/** @defgroup highlevel Serial Flash High Level API
 *
 * This set of functions provide simple sequential read/write operations.
 * The read/write functions behave similarly to standard file operations.
 * The driver maintains a pointer to the position on the memory where the next
 * read/write operation should take place. It also manages a single page
 * buffer which reduces the access to the flash improving the overall 
 * performance.
 *
 * The functions are suitable for a simple pseudo filesystem, where
 * the files are prealocated in partitions, with single write and multiple 
 * reads.
 *
 * @{
 */

/** @brief Erase a partition of arbitrary size.
 *
 * This function will erase a contiguous flash region starting at
 * the sector containing the address @p addr up to the sector
 * containing the address (@p addr + @p count). If @p addr
 * or @p count are not sector aligned, the total erased bytes will
 * be larger than the requested. This will be indicated in the
 * return value of the function.
 *
 * @param sf The serial flash device.
 * @param addr The initial partition address.
 * @param count Minimum number of bytes to erase.
 * @return On success, the number of bytes erased is returned.
 * On error @b -1 is returned.
 */
int sflash_erase(struct sflash_dev * sf, uint32_t addr, size_t count);

/** @brief Read from flash.
 *
 * This function will try to read up to @p count bytes from the flash
 * into the buffer pointed by @p buf.
 *
 * @param sf The serial flash device.
 * @param buf Pointer to a buffer to receive the data.
 * @param count Number of bytes to read.
 * @return On success, the number of bytes read is returned.
 * On error @b -1 is returned.
 */
int sflash_read(struct sflash_dev * sf, void * buf, size_t count);

/** @brief Write to flash.
 *
 * This function writes up to @p count bytes from the buffer pointed 
 * by @p buf int the flash.
 *
 * @param sf The serial flash device.
 * @param buf Pointer to the data.
 * @param count Number of bytes to write.
 * @return On success, the number of bytes written is returned.
 * On error @b -1 is returned.
 */
int sflash_write(struct sflash_dev * sf, const void * buf, size_t count);

/** @brief Synchronize changes to flash.
 *
 * The @c sflash_write() function shall request that all data
 * buffered by the serial flash driver is to be transfered to
 * the physical device.
 *
 * @param sf The serial flash device.
 * @return On success, 0 is returned.
 * On error @b -1 is returned.
 */
int sflash_sync(struct sflash_dev * sf);

/** @brief Reposition read/write pointer.
 *
 * The @c sflash_seek() repositions the internal pointer
 * associated with the serial flash driver.
 *
 * @param sf The serial flash device.
 * @param pos The new address for subsequent read/write.
 * @return On success, 0 is returned.
 * On error @b -1 is returned.
 */
int sflash_seek(struct sflash_dev * sf, unsigned int pos);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* __SFLASH_H__ */

