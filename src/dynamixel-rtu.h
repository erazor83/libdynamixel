/*
 * Copyright (C) 2013 Alexander Krause <alexander.krause@ed-solutions.de>
 * 
 * Dynamixel library - a fork from libmodbus (http://libmodbus.org)
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#ifndef _DYNAMIXEL_RTU_H_
#define _DYNAMIXEL_RTU_H_

#include "dynamixel.h"

DYNAMIXEL_BEGIN_DECLS

/* Modbus_Application_Protocol_V1_1b.pdf Chapter 4 Section 1 Page 5
 * RS232 / RS485 ADU = 253 bytes + slave (1 byte) + CRC (2 bytes) = 256 bytes
 */
#define DYNAMIXEL_RTU_MAX_ADU_LENGTH  255

dynamixel_t* dynamixel_new_rtu(const char *device, uint32_t baud, char parity,
                         uint8_t data_bit, uint8_t stop_bit);

#define DYNAMIXEL_RTU_RS232 0
#define DYNAMIXEL_RTU_RS485 1

int8_t dynamixel_rtu_set_serial_mode(dynamixel_t *ctx, int8_t);
int8_t dynamixel_rtu_get_serial_mode(dynamixel_t *ctx);

int8_t _dynamixel_rtu_flush(dynamixel_t *);

DYNAMIXEL_END_DECLS

#endif /* _DYNAMIXEL_RTU_H_ */
