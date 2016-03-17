/* Copyright (c) 2016 Eetu "Razbit" Pesonen
 *
 * This file is part of RazOS.
 *
 * RazOS is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * RazOS is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RazOS. If not, see <http://www.gnu.org/licenses/>. */

#include <time.h>

#include <sys/types.h>
#include <portio.h>
#include <string.h>
#include <console.h>

static uint8_t cmos_read(uint8_t index)
{
	outb(0x70, index);
	return inb(0x71);
}

static struct tm* read_rtc()
{
	static struct tm cur_tm;
	struct tm last_tm;

	/* We read the RTC (at least) twice, to make sure we won't get any
	 * dodgy values while the RTC is updating */

	/* Make sure the RTC is not in the middle of an update */
	while (cmos_read(0x0A) & 0x80); /* bit 7 of status reg A */

	cur_tm.tm_sec = (int)cmos_read(0x00);
	cur_tm.tm_min = (int)cmos_read(0x02);
	cur_tm.tm_hour = (int)cmos_read(0x04);
	cur_tm.tm_mday = (int)cmos_read(0x07);
	cur_tm.tm_mon = (int)cmos_read(0x08);
	cur_tm.tm_year = (int)cmos_read(0x09);

	do
	{
		last_tm = cur_tm;

		while (cmos_read(0x0A) & 0x80); /* bit 7 of status reg A */

		cur_tm.tm_sec = (int)cmos_read(0x00);
		cur_tm.tm_min = (int)cmos_read(0x02);
		cur_tm.tm_hour = (int)cmos_read(0x04);
		cur_tm.tm_mday = (int)cmos_read(0x07);
		cur_tm.tm_mon = (int)cmos_read(0x08);
		cur_tm.tm_year = (int)cmos_read(0x09);

	} while (memcmp(&cur_tm, &last_tm, sizeof(struct tm)) != 0);


	/* RTC values may be in BCD format, check status reg B bit 2 */
	uint8_t status_reg_b = cmos_read(0x0B);
	if (!(status_reg_b & 0x04))
	{
		cur_tm.tm_sec = (cur_tm.tm_sec&0x0F)+((cur_tm.tm_sec/16)*10);
		cur_tm.tm_min = (cur_tm.tm_min&0x0F)+((cur_tm.tm_min/16)*10);
		cur_tm.tm_hour = (cur_tm.tm_hour&0x0F)+((cur_tm.tm_hour/16)*10);
		cur_tm.tm_mday = (cur_tm.tm_mday&0x0F)+((cur_tm.tm_mday/16)*10);
		cur_tm.tm_mon = (cur_tm.tm_mon&0x0F)+((cur_tm.tm_mon/16)*10);
		cur_tm.tm_year = (cur_tm.tm_year&0x0F)+((cur_tm.tm_year/16)*10);
	}

	if (!(status_reg_b & 0x02) && (cur_tm.tm_hour & 0x80))
		cur_tm.tm_hour = ((cur_tm.tm_hour & 0x7F) + 12) % 24;

	/* RTC only give us the two least-significant digits of year,
	 * century register is not standardized. So, we assume we haven't
	 * gone back in time by adding 2000 to tm_year. Since tm struct
	 * saves years as offset from 1900, add 100 (2000 - 1900 = 100) */
	cur_tm.tm_year += 100;

	/* tm struct saves months [0,11], not [1,12] */
	cur_tm.tm_mon--;

	return &cur_tm;
}

/* Get seconds since epoch. Return to *timer too, if not NULL */
time_t time(time_t* timer)
{
	time_t ut = mktime(read_rtc());
	if (timer != NULL)
		*timer = ut;
	return ut;
}
