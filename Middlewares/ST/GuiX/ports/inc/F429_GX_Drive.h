#ifndef __F429_GX_DRIVE_H__
#define __F429_GX_DRIVE_H__
#include 	 "main.h"
#include   "tx_api.h"
#include   "gx_api.h"
#include   "gx_system.h"
#include   "gx_display.h"
#include   "gx_utility.h"
#include   "touch_480x272.h"

#define STM32_SCREEN_HANDLE   0xD0000000
#define FrameBufer   0xD0000000

UINT stm32f4_graphics_driver_setup_24xrgb(GX_DISPLAY *display);
void TouchDrive(void);
#endif