/* Compile-only checks that are not covered by compiling the library sources. */
#include "hwdefs.h"
#include "delay.h"
#include "stm32_loader.h"

void family_delay_compiles(void)
{
   uDelay(1);
}

#if !defined(STM32F4)
static_assert(PINDEF_BLKNUM == 3, "F1 pindef page index changed");
static_assert(PINDEF_BLKSIZE == 1024, "F1 pindef page size changed");
#else
static_assert(PINDEF_ADDRESS == 0x0800C000u, "compile-check pindef address");
static_assert(PINDEF_SECTOR == 3u, "compile-check pindef sector");
static_assert(DELAY_CYCLES_PER_US == 28, "F4 delay default should match 168 MHz");
#endif
