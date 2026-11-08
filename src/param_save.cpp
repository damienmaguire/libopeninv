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

#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/desig.h>
#include <libopencm3/stm32/crc.h>
#include "params.h"
#include "param_save.h"
#include "hwdefs.h"
#include "my_string.h"
#if defined(STM32F4)
#include "stm32_flash.h"
#ifndef PARAM_ADDRESS
#error STM32F4 parameter save needs PARAM_ADDRESS from the application.
#endif
#ifndef PARAM_SECTOR
#error STM32F4 parameter save needs PARAM_SECTOR from the application.
#endif
#if defined(CANMAP_SECTOR) && (CANMAP_SECTOR == PARAM_SECTOR)
#error Parameter store and CAN map share one STM32F4 sector. Give each store its own sector.
#endif
static_assert(F4_STORE_FITS(PARAM_ADDRESS, PARAM_BLKSIZE, PARAM_SECTOR),
   "Parameter store is outside its sector or overlaps the application image");
#endif

#define NUM_PARAMS ((PARAM_BLKSIZE - 8) / sizeof(PARAM_ENTRY))
#define PARAM_WORDS (PARAM_BLKSIZE / 4)

typedef struct
{
   uint16_t key;
   uint8_t dummy;
   uint8_t flags;
   uint32_t value;
} PARAM_ENTRY;

typedef struct
{
   PARAM_ENTRY data[NUM_PARAMS];
   uint32_t crc;
   uint32_t padding;
} PARAM_PAGE;

static uint32_t GetFlashAddress()
{
#if defined(STM32F4)
   return PARAM_ADDRESS;
#else
   uint32_t flashSize = desig_get_flash_size();

   //Always save parameters to last flash page
   return FLASH_BASE + flashSize * 1024 - PARAM_BLKNUM * PARAM_BLKSIZE;
#endif
}

/**
* Save parameters to flash
*
* @return CRC of parameter flash page.
*         On STM32F4, 0 means the store was refused and flash was not programmed.
*/
uint32_t parm_save()
{
   PARAM_PAGE parmPage;
   uint32_t idx;
   uint32_t paramAddress = GetFlashAddress();
#if !defined(STM32F4)
   uint32_t check = 0xFFFFFFFF;
   uint32_t* baseAddress = (uint32_t*)paramAddress;

   for (int i = 0; i < PARAM_WORDS; i++, baseAddress++)
      check &= *baseAddress;
#endif

   crc_reset();
   memset32((int*)&parmPage, 0xFFFFFFFF, PARAM_WORDS);

   //Copy parameter values and keys to block structure
   for (idx = 0; idx < NUM_PARAMS && idx < Param::PARAM_LAST; idx++)
   {
      if (Param::GetType((Param::PARAM_NUM)idx) == Param::TYPE_PARAM)
      {
         const Param::Attributes *pAtr = Param::GetAttrib((Param::PARAM_NUM)idx);
         parmPage.data[idx].flags = (uint8_t)Param::GetFlag((Param::PARAM_NUM)idx);
         parmPage.data[idx].key = pAtr->id;
         parmPage.data[idx].value = Param::Get((Param::PARAM_NUM)idx);
      }
   }

   parmPage.crc = crc_calculate_block(((uint32_t*)&parmPage), (2 * NUM_PARAMS));
   flash_unlock();

#if defined(STM32F4)
   /* Erase only when this store is not blank, and only its own sector.
    * Do not change the flash wait states here; the application set those
    * for the running clock.
    */
   if (!f4_prepare_store(paramAddress, PARAM_BLKSIZE, PARAM_SECTOR) ||
       !f4_program_words(paramAddress, (uint32_t*)&parmPage, PARAM_WORDS))
   {
      flash_lock();
      return 0;
   }
#else
   if (check != 0xFFFFFFFF)
      flash_erase_page(paramAddress);

   for (idx = 0; idx < PARAM_WORDS; idx++)
   {
      uint32_t* pData = ((uint32_t*)&parmPage) + idx;
      flash_program_word(paramAddress + idx * sizeof(uint32_t), *pData);
   }
#endif
   flash_lock();
   return parmPage.crc;
}

/**
* Load parameters from flash
*
* @retval 0 Parameters loaded successfully
* @retval -1 CRC error, parameters not loaded
*/
int parm_load()
{
   uint32_t paramAddress = GetFlashAddress();
   PARAM_PAGE *parmPage = (PARAM_PAGE *)paramAddress;

   crc_reset();
   uint32_t crc = crc_calculate_block(((uint32_t*)parmPage), (2 * NUM_PARAMS));

   if (crc == parmPage->crc)
   {
      for (unsigned int idxPage = 0; idxPage < NUM_PARAMS; idxPage++)
      {
         Param::PARAM_NUM idx = Param::NumFromId(parmPage->data[idxPage].key);
         if (idx != Param::PARAM_INVALID && Param::GetType((Param::PARAM_NUM)idx) == Param::TYPE_PARAM)
         {
            Param::SetFixed(idx, parmPage->data[idxPage].value);
            Param::SetFlagsRaw(idx, parmPage->data[idxPage].flags);
         }
      }
      return 0;
   }

   return -1;
}
