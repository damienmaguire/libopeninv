/*
 * This file is part of the libopeninv project.
 *
 * Copyright (C) 2010 Johannes Huebner <contact@johanneshuebner.com>
 * Copyright (C) 2010 Edward Cheeseman <cheesemanedward@gmail.com>
 * Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
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
#include "digio.h"

#define DIG_IO_OFF 0
#define DIG_IO_ON  1

#undef DIG_IO_ENTRY
#define DIG_IO_ENTRY(name, port, pin, mode) DigIo DigIo::name;

#ifdef BUSIO_ENABLED    
#undef  BUS_IO_ENTRY
#define BUS_IO_ENTRY(name, busType, channel, mode) BusIo DigIo::name;
#endif

DIG_IO_LIST

void DigIo::Configure(uint32_t port, uint16_t pin, PinMode::PinMode pinMode)
{
#if defined(STM32F4)
   // F4 has separate bits for the pull, the output type and the mode.
   // The alternate-function number is selected by the peripheral driver.
   uint8_t mode = GPIO_MODE_INPUT;
   uint8_t pull = GPIO_PUPD_PULLDOWN;
   uint8_t type = GPIO_OTYPE_PP;
   bool setHigh = false;

   _port = port;
   _pin = pin;
   _invert = 0;

   switch (pinMode)
   {
      default:
      case PinMode::INPUT_PD:
         break;
      case PinMode::INPUT_PD_INV:
         _invert = 1;
         break;
      case PinMode::INPUT_PU:
         pull = GPIO_PUPD_PULLUP;
         break;
      case PinMode::INPUT_PU_INV:
         pull = GPIO_PUPD_PULLUP;
         _invert = 1;
         break;
      case PinMode::INPUT_FLT:
         pull = GPIO_PUPD_NONE;
         break;
      case PinMode::INPUT_FLT_INV:
         pull = GPIO_PUPD_NONE;
         _invert = 1;
         break;
      case PinMode::INPUT_AIN:
         mode = GPIO_MODE_ANALOG;
         pull = GPIO_PUPD_NONE;
         break;
      case PinMode::OUTPUT:
         mode = GPIO_MODE_OUTPUT;
         pull = GPIO_PUPD_NONE;
         break;
      case PinMode::OUTPUT_OD:
         mode = GPIO_MODE_OUTPUT;
         pull = GPIO_PUPD_NONE;
         type = GPIO_OTYPE_OD;
         setHigh = true;
         break;
      case PinMode::OUTPUT_ALT:
         mode = GPIO_MODE_AF;
         pull = GPIO_PUPD_NONE;
         break;
   }

   // Match the F1 path: set the output latch before changing the mode.
   if (setHigh)
      gpio_set(port, pin);
   if (mode == GPIO_MODE_OUTPUT || mode == GPIO_MODE_AF)
      gpio_set_output_options(port, type, GPIO_OSPEED_50MHZ, pin);
   gpio_mode_setup(port, mode, pull, pin);
#else
   uint8_t mode = GPIO_MODE_INPUT;
   uint8_t cnf = GPIO_CNF_INPUT_PULL_UPDOWN;
   uint16_t val = DIG_IO_OFF;

   _port = port;
   _pin = pin;
   _invert = 0;

   switch (pinMode)
   {
      default:
      case PinMode::INPUT_PD:
         /* use defaults */
         break;
      case PinMode::INPUT_PD_INV:
         /* use defaults */
         _invert = 1;
         break;
      case PinMode::INPUT_PU:
         val = DIG_IO_ON;
         break;
      case PinMode::INPUT_PU_INV:
         val = DIG_IO_ON;
         _invert = 1;
         break;
      case PinMode::INPUT_FLT:
         cnf = GPIO_CNF_INPUT_FLOAT;
         break;
      case PinMode::INPUT_FLT_INV:
         cnf = GPIO_CNF_INPUT_FLOAT;
         _invert = 1;
         break;
      case PinMode::INPUT_AIN:
         cnf = GPIO_CNF_INPUT_ANALOG;
         break;
      case PinMode::OUTPUT:
         mode = GPIO_MODE_OUTPUT_50_MHZ;
         cnf = GPIO_CNF_OUTPUT_PUSHPULL;
         break;
      case PinMode::OUTPUT_OD:
         mode = GPIO_MODE_OUTPUT_50_MHZ;
         cnf = GPIO_CNF_OUTPUT_OPENDRAIN;
         val = DIG_IO_ON;
         break;
      case PinMode::OUTPUT_ALT:
         mode = GPIO_MODE_OUTPUT_50_MHZ;
         cnf = GPIO_CNF_OUTPUT_ALTFN_PUSHPULL;
         break;
   }

   if (DIG_IO_ON == val)
   {
      gpio_set(port, pin);
   }
   gpio_set_mode(port, mode, cnf, pin);
#endif
}

