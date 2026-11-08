/*
 * This file is part of the stm32-sine project.
 *
 * Copyright (C) 2021 David J. Fiddes <D.J@fiddes.net>
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
#ifndef DELAY_H
#define DELAY_H

/**
 * \brief Blocking delay for a period
 *
 * \param[in] period Length of the delay in micro-seconds
 */
/* Busy-loop iterations per microsecond.
 * 12 was measured by toggling a pin for 1000 us on a 72 MHz STM32F103.
 * The F4 default scales that measurement to a 168 MHz SYSCLK
 * (12 * 168 / 72 = 28). Define DELAY_CYCLES_PER_US before including
 * this header when the clock is different. This is still a busy loop.
 */
#ifndef DELAY_CYCLES_PER_US
#if defined(STM32F4)
#define DELAY_CYCLES_PER_US 28
#else
#define DELAY_CYCLES_PER_US 12
#endif
#endif

inline void uDelay(int period)
{
    static const int CyclesPerMicroSecond = DELAY_CYCLES_PER_US;

    int iterations = period * CyclesPerMicroSecond;

    for (int i = 0; i < iterations; i++)
    {
        __asm__("nop");
    }
}

#endif // DELAY_H
