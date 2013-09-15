/*
 * Copyright © 2001-2011 Stéphane Raimbault <stephane.raimbault@gmail.com>
 * Modified for Dynamixel 2013 Alexander Krause <alexander.krause@ed-solutions.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef _DYNAMIXEL_H_
#define _DYNAMIXEL_H_

/* Add this for macros that defined unix flavor */
#if (defined(__unix__) || defined(unix)) && !defined(USG)
#include <sys/param.h>
#endif

#ifndef _MSC_VER
#include <stdint.h>
#include <sys/time.h>
#else
#include "stdint.h"
#include <time.h>
#endif

#include "dynamixel-version.h"

#ifdef  __cplusplus
# define DYNAMIXEL_BEGIN_DECLS  extern "C" {
# define DYNAMIXEL_END_DECLS    }
#else
# define DYNAMIXEL_BEGIN_DECLS
# define DYNAMIXEL_END_DECLS
#endif

DYNAMIXEL_BEGIN_DECLS

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef OFF
#define OFF 0
#endif

#ifndef ON
#define ON 1
#endif

#define DYNAMIXEL_BROADCAST_ADDRESS    0xFE

/* Native libdynamixel error codes */
#define E_DYNAMIXEL_BADCRC  1
#define E_DYNAMIXEL_BADDATA 2
#define E_DYNAMIXEL_BADEXC  3
#define E_DYNAMIXEL_UNKEXC  4
#define E_DYNAMIXEL_MDATA   5


extern const unsigned int libdynamixel_version_major;
extern const unsigned int libdynamixel_version_minor;
extern const unsigned int libdynamixel_version_micro;

typedef struct _dynamixel dynamixel_t;

typedef enum
{
    DYNAMIXEL_ERROR_RECOVERY_NONE          = 0,
    DYNAMIXEL_ERROR_RECOVERY_LINK          = (1<<1),
    DYNAMIXEL_ERROR_RECOVERY_PROTOCOL      = (1<<2),
} dynamixel_error_recovery_mode;

typedef enum {
    /* Request message on the server side */
    MSG_INDICATION,
    /* Request message on the client side */
    MSG_CONFIRMATION
} msg_type_t;

int dynamixel_set_error_recovery(dynamixel_t *ctx, dynamixel_error_recovery_mode error_recovery);

#define DYNAMIXEL_ERROR_BIT_0			"Input Voltage Error"
#define DYNAMIXEL_ERROR_BIT_1			"Angle Limit Error"
#define DYNAMIXEL_ERROR_BIT_2			"Overheating Error"
#define DYNAMIXEL_ERROR_BIT_3			"Range Error"
#define DYNAMIXEL_ERROR_BIT_4			"Checksum Error"
#define DYNAMIXEL_ERROR_BIT_5			"Overload Error"
#define DYNAMIXEL_ERROR_BIT_6			"Instruction Error "
#define DYNAMIXEL_ERROR_BIT_7			"0"

void dynamixel_get_response_timeout(dynamixel_t *ctx, struct timeval *timeout);
void dynamixel_set_response_timeout(dynamixel_t *ctx, const struct timeval *timeout);

void dynamixel_get_byte_timeout(dynamixel_t *ctx, struct timeval *timeout);
void dynamixel_set_byte_timeout(dynamixel_t *ctx, const struct timeval *timeout);

int dynamixel_get_header_length(dynamixel_t *ctx);

/**
 * UTILS FUNCTIONS
 **/

#define DYNAMIXEL_GET_HIGH_BYTE(data) (((data) >> 8) & 0xFF)
#define DYNAMIXEL_GET_LOW_BYTE(data) ((data) & 0xFF)
#define DYNAMIXEL_GET_INT32_FROM_INT16(tab_int16, index) ((tab_int16[(index)] << 16) + tab_int16[(index) + 1])
#define DYNAMIXEL_GET_INT16_FROM_INT8(tab_int8, index) ((tab_int8[(index)] << 8) + tab_int8[(index) + 1])
#define DYNAMIXEL_SET_INT16_TO_INT8(tab_int8, index, value) \
    do { \
        tab_int8[(index)] = (value) >> 8;  \
        tab_int8[(index) + 1] = (value) & 0xFF; \
    } while (0)

void dynamixel_set_bits_from_byte(uint8_t *dest, int index, const uint8_t value);
void dynamixel_set_bits_from_bytes(uint8_t *dest, int index, unsigned int nb_bits,
                                const uint8_t *tab_byte);
uint8_t dynamixel_get_byte_from_bits(const uint8_t *src, int index, unsigned int nb_bits);
float dynamixel_get_float(const uint16_t *src);
void dynamixel_set_float(float f, uint16_t *dest);

void dynamixel_close(dynamixel_t *);

#include "dynamixel-rtu.h"

DYNAMIXEL_END_DECLS

#endif  /* _DYNAMIXEL_H_ */
