/*
 * This file is part of the libopeninv project.
 *
 * Copyright (C) 2011 Johannes Huebner <dev@johanneshuebner.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef STM32_FLASH_H
#define STM32_FLASH_H

/* STM32F4 parameter and CAN-map storage.
 *
 * The application chooses the address and the sector. This file checks
 * that the store sits inside that sector and outside the application
 * image. It does not invent a layout, and it does not erase a sector
 * just because the address is "near the end of flash".
 *
 * The sector table below is the STM32F405/F407 single-bank map
 * (16 KiB, 16 KiB, 16 KiB, 16 KiB, 64 KiB, then 128 KiB). The sector
 * number is the one passed to libopencm3 flash_erase_sector().
 * Give each store its own sector: erasing one store wipes the sector.
 */

#include <stdint.h>
#include <libopencm3/stm32/flash.h>

#if !defined(STM32F4)
#error stm32_flash.h is only for STM32F4. The F1 code erases pages.
#endif

#ifndef FLASH_APP_START
#error Define FLASH_APP_START, the first address of the application image.
#endif
#ifndef FLASH_APP_END
#error Define FLASH_APP_END, one past the last address of the application image.
#endif

#define F4_FLASH_ERR (FLASH_SR_WRPERR | FLASH_SR_PGAERR | FLASH_SR_PGPERR | \
   FLASH_SR_OPERR | FLASH_SR_PGSERR)

/* Offset of a sector from FLASH_BASE. Unknown sectors return a value
 * that cannot match a real store address.
 */
#define F4_SECTOR_OFFSET(n) ( \
   ((n) == 0) ? 0x00000u : \
   ((n) == 1) ? 0x04000u : \
   ((n) == 2) ? 0x08000u : \
   ((n) == 3) ? 0x0C000u : \
   ((n) == 4) ? 0x10000u : \
   ((n) == 5) ? 0x20000u : \
   ((n) == 6) ? 0x40000u : \
   ((n) == 7) ? 0x60000u : \
   ((n) == 8) ? 0x80000u : \
   ((n) == 9) ? 0xA0000u : \
   ((n) == 10) ? 0xC0000u : \
   ((n) == 11) ? 0xE0000u : \
   0xFFFFFFFFu)

#define F4_SECTOR_BYTES(n) ( \
   ((n) <= 3) ? 0x4000u : \
   ((n) == 4) ? 0x10000u : \
   ((n) <= 11) ? 0x20000u : \
   0u)

#define F4_RANGE_OVERLAPS(a, asz, b, bsz) \
   (((uint32_t)(a) < ((uint32_t)(b) + (uint32_t)(bsz))) && \
    (((uint32_t)(a) + (uint32_t)(asz)) > (uint32_t)(b)))

/* True when [addr, addr + size) is inside the named sector, 4-byte
 * aligned, and does not meet [FLASH_APP_START, FLASH_APP_END).
 */
#define F4_STORE_FITS(addr, size, sector) \
   ((uint32_t)(size) > 0u && \
    ((uint32_t)(size) % 4u) == 0u && \
    ((uint32_t)(size) <= F4_SECTOR_BYTES(sector)) && \
    ((uint32_t)(addr) % 4u) == 0u && \
    (uint32_t)(sector) <= 11u && \
    (uint32_t)(addr) >= (FLASH_BASE + F4_SECTOR_OFFSET(sector)) && \
    ((uint32_t)(addr) - (FLASH_BASE + F4_SECTOR_OFFSET(sector))) <= \
       (F4_SECTOR_BYTES(sector) - (uint32_t)(size)) && \
    (FLASH_APP_END > FLASH_APP_START) && \
    !F4_RANGE_OVERLAPS((addr), (size), FLASH_APP_START, \
       (FLASH_APP_END - FLASH_APP_START)))

static inline bool f4_words_blank(uint32_t address, uint32_t words)
{
   volatile uint32_t *word = reinterpret_cast<volatile uint32_t *>(address);

   for (uint32_t i = 0; i < words; i++)
   {
      if (word[i] != 0xffffffffu)
         return false;
   }
   return true;
}

/* Erase the store's sector when any word of the store is not blank.
 * Returns false without touching flash when the address and sector disagree.
 */
static inline bool f4_prepare_store(uint32_t address, uint32_t bytes, unsigned sector)
{
   if (!F4_STORE_FITS(address, bytes, sector))
      return false;

   flash_clear_status_flags();
   FLASH_SR |= FLASH_SR_OPERR;

   if (!f4_words_blank(address, bytes / 4u))
   {
      flash_erase_sector((uint8_t)sector, FLASH_CR_PROGRAM_X32);
      if ((FLASH_SR & F4_FLASH_ERR) != 0)
         return false;
      if (!f4_words_blank(address, bytes / 4u))
         return false;
   }
   return true;
}

static inline bool f4_program_words(uint32_t address, const uint32_t *data, uint32_t words)
{
   for (uint32_t i = 0; i < words; i++)
   {
      flash_program_word(address + i * 4u, data[i]);
      if ((FLASH_SR & F4_FLASH_ERR) != 0)
         return false;
   }
   return true;
}

#endif // STM32_FLASH_H
