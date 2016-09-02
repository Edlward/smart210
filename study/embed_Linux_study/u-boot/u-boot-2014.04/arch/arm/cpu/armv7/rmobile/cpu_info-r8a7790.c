/*
 * arch/arm/cpu/armv7/rmobile/cpu_info-r8a7790.c
 *     This file is r8a7790 processor support.
 *
 * Copyright (C) 2013 Renesas Electronics Corporation
 *
 * SPDX-License-Identifier: GPL-2.0
 */
#include <common.h>
#include <asm/io.h>

#define PRR 0xFF000044

u32 rmobile_get_cpu_type(void)
{
	return (readl(PRR) & 0x00007F00) >> 8;
}

u32 rmobile_get_cpu_rev_integer(void)
{
	return (readl(PRR) & 0x000000F0) >> 4;
}
