/*
 * This file is part of the libopeninv project.
 *
 * Copyright (C) 2016 Nail Güzel
 * Johannes Huebner <dev@johanneshuebner.com>
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
#ifndef STM32_CAN_H_INCLUDED
#define STM32_CAN_H_INCLUDED
#include "canhardware.h"

/* Seconds counter stamped onto each received CAN frame.
 *
 * On STM32F1 a weak definition in stm32_can.cpp reads the RTC counter,
 * so existing applications keep the old timestamp without changes.
 * STM32F4 has no compatible counter. The application provides this
 * function, usually from the same seconds timebase it uses for
 * timeouts. Do not wrap rtc_get_time(): that returns calendar fields.
 *
 * The symbol has C linkage. Defining a macro named
 * libopeninv_get_seconds before this header replaces the call.
 */
#ifndef libopeninv_get_seconds
extern "C" uint32_t libopeninv_get_seconds(void);
#endif

#ifndef SENDBUFFER_LEN
#define SENDBUFFER_LEN 20
#endif // SENDBUFFER_LEN

class Stm32Can: public CanHardware
{
public:
   Stm32Can(uint32_t baseAddr, enum baudrates baudrate, bool remap = false);
   void SetBaudrate(enum baudrates baudrate);
   void Send(uint32_t canId, uint32_t data[2], uint8_t len, bool forceExt);
   void HandleTx();
   void HandleMessage(int fifo);
   static Stm32Can* GetInterface(int index);

private:
   struct SENDBUFFER
   {
      uint32_t id;
      bool forceExt;
      uint8_t len;
      uint32_t data[2];
   };

   SENDBUFFER sendBuffer[SENDBUFFER_LEN];
   int sendCnt;
   uint32_t canDev;

   void ConfigureFilters();
   void SetFilterBank(int& idIndex, int& filterId, uint16_t* idList);
   void SetFilterBankMask(int& idIndex, int& filterId, uint16_t* idMaskList);
   void SetFilterBank29(int& idIndex, int& filterId, uint32_t* idList);

   static Stm32Can* interfaces[];
};


#endif
