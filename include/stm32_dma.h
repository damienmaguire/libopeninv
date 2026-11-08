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
#ifndef STM32_DMA_H
#define STM32_DMA_H

#include <libopencm3/stm32/dma.h>

/* Small family wrapper so terminal and LIN can share one call site.
 * On F1 the id is a DMA channel. On F4 it is a stream number.
 */

#if defined(STM32F4)

/* F4 clears the stream enable bit only after the current transfer stops.
 * The stream registers ignore writes until that bit reads zero.
 */
static inline void dma_stop(uint32_t dma, uint8_t stream)
{
   dma_disable_stream(dma, stream);
   while (DMA_SCR(dma, stream) & DMA_SxCR_EN)
      ;
}

static inline void dma_start(uint32_t dma, uint8_t stream)
{
   dma_enable_stream(dma, stream);
}

#else

static inline void dma_stop(uint32_t dma, uint8_t channel)
{
   dma_disable_channel(dma, channel);
}

static inline void dma_start(uint32_t dma, uint8_t channel)
{
   dma_enable_channel(dma, channel);
}

#endif

#endif // STM32_DMA_H
