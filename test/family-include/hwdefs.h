/*
 * Hardware definitions for the F1/F4 compile check only.
 * A real board supplies its own hwdefs.h. The addresses below are an
 * example of separate F405/F407 sectors, not a ZombieVerter decision.
 */
#ifndef HWDEFS_H
#define HWDEFS_H

#define PARAM_BLKSIZE 1024
#define PARAM_BLKNUM 1
#define FLASH_PAGE_SIZE 2048
#define CAN1_BLKNUM 2

#if defined(STM32F4)
#define PARAM_ADDRESS   0x08004000u
#define PARAM_SECTOR    1u
#define CANMAP_ADDRESS  0x08008000u
#define CANMAP_SECTOR   2u
#define FLASH_APP_START 0x08010000u
#define FLASH_APP_END   0x08100000u
#define PINDEF_ADDRESS  0x0800C000u
#define PINDEF_SECTOR   3u
#endif

#endif
