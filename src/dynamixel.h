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
#ifndef _DYNAMIXEL_H_
#define _DYNAMIXEL_H_

/* Add this for macros that defined unix flavor */
#if (defined(__unix__) || defined(unix)) && !defined(USG)
#include <sys/param.h>
#endif

#ifndef _MSC_VER
#ifndef _SWIG
#include <stdint.h>
#include <sys/time.h>
#endif
#else
#include "stdint.h"
#include <time.h>
#endif

#ifndef _SWIG
#include <stdbool.h>
#endif

#include <dynamixel_config.h>

#ifdef  __cplusplus
# define DYNAMIXEL_BEGIN_DECLS  extern "C" {
# define DYNAMIXEL_END_DECLS    }
#else
# define DYNAMIXEL_BEGIN_DECLS
# define DYNAMIXEL_END_DECLS
#endif

#ifndef _SWIG
#define _DYNAMIXEL_SERIAL_DEFAULTS	'N',8,1
#endif

#ifndef OFF
#define OFF 0
#endif

#ifndef ON
#define ON 1
#endif

DYNAMIXEL_BEGIN_DECLS

#define DYNAMIXEL_MAX_PARAMETER_COUNT  245

#define DYNAMIXEL_BROADCAST_ADDRESS    0xFE

/* Native libdynamixel error codes */
#define E_DYNAMIXEL_BADCHECKSUM  1
#define E_DYNAMIXEL_BADDATA			 2
#define E_DYNAMIXEL_BADEXC 			 3
#define E_DYNAMIXEL_UNKEXC 			 4
#define E_DYNAMIXEL_MDATA  			 5


#define DYNAMIXEL_ERROR_BIT_0			"Input Voltage Error"
#define DYNAMIXEL_ERROR_BIT_1			"Angle Limit Error"
#define DYNAMIXEL_ERROR_BIT_2			"Overheating Error"
#define DYNAMIXEL_ERROR_BIT_3			"Range Error"
#define DYNAMIXEL_ERROR_BIT_4			"Checksum Error"
#define DYNAMIXEL_ERROR_BIT_5			"Overload Error"
#define DYNAMIXEL_ERROR_BIT_6			"Instruction Error "
#define DYNAMIXEL_ERROR_BIT_7			"0"


typedef enum {
#ifdef HAVE_BAUDRATE_1000000
	DYNAMIXEL_BR_1MBPS						= 1,
#endif
#ifdef HAVE_BAUDRATE_500000
	DYNAMIXEL_BR_500KBPS					= 3,
#endif
#ifdef HAVE_BAUDRATE_400000
	DYNAMIXEL_BR_400KBPS					= 4,
#endif
#ifdef HAVE_BAUDRATE_250000
	DYNAMIXEL_BR_250KBPS					= 7,
#endif
#ifdef HAVE_BAUDRATE_200000
	DYNAMIXEL_BR_200KBPS					= 9,
#endif
	DYNAMIXEL_BR_117KBPS					= 16,
	DYNAMIXEL_BR_57KBPS						= 34,
	DYNAMIXEL_BR_19KBPS						= 103,
	DYNAMIXEL_BR_9KBPS						= 207,
} dynamixel_baudrate_t;

typedef enum {
	DYNAMIXEL_R_MODELNUMBER_L						= 0x00,
	DYNAMIXEL_R_MODELNUMBER_H						= 0x01,
	DYNAMIXEL_R_VERSIONOFFIRMWARE				= 0x02,
	DYNAMIXEL_R_ID											= 0x03,
	DYNAMIXEL_R_BAUDRATE								= 0x04,
	DYNAMIXEL_R_RETURNDELAY_TIME				= 0x05,
	DYNAMIXEL_R_CW_ANGLELIMIT_L					= 0x06,
	DYNAMIXEL_R_CW_ANGLELIMIT_H					= 0x07,
	DYNAMIXEL_R_CCW_ANGLELIMIT_L				= 0x08,
	DYNAMIXEL_R_CCW_ANGLELIMIT_H				= 0x09,
	DYNAMIXEL_R_LIMIT_TEMPERATURE_HIGH	= 0x0B,
	DYNAMIXEL_R_LIMIT_VOLTAGE_LOW				= 0x0C,
	DYNAMIXEL_R_LIMIT_VOLTAGE_HIGH			= 0x0D,
	DYNAMIXEL_R_MAX_TORQUE_L						= 0x0E,
	DYNAMIXEL_R_MAX_TORQUE_H						= 0x0F,
	DYNAMIXEL_R_STATUS_RETURN_LEVEL			= 0x10,
	DYNAMIXEL_R_ALARM_LED								= 0x11,
	DYNAMIXEL_R_ALARM_SHUTDOWN					= 0x12,
	DYNAMIXEL_R_DOWN_CALIBRATION_L			= 0x14,
	DYNAMIXEL_R_DOWN_CALIBRATION_H			= 0x15,
	DYNAMIXEL_R_UP_CALIBRATION_L				= 0x16,
	DYNAMIXEL_R_UP_CALIBRATION_H				= 0x17,
	DYNAMIXEL_R_TORQUE_ENABLE						= 0x18,
	DYNAMIXEL_R_LED											= 0x19,
	DYNAMIXEL_R_CW_COMPLIANCE_MARGIN		= 0x1A,
	DYNAMIXEL_R_CCW_COMPLIANCE_MARGIN		= 0x1B,
	DYNAMIXEL_R_CW_COMPLIANCE_SLOPE			= 0x1C,
	DYNAMIXEL_R_CCW_COMPLIANCE_SLOPE		= 0x1D,
	DYNAMIXEL_R_GOAL_POSITION_L					= 0x1E,
	DYNAMIXEL_R_GOAL_POSITION_H					= 0x1F,
	DYNAMIXEL_R_MOVING_SPEED_L					= 0x20,
	DYNAMIXEL_R_MOVING_SPEED_H					= 0x21,
	DYNAMIXEL_R_TORQUE_LIMIT_L					= 0x22,
	DYNAMIXEL_R_TORQUE_LIMIT_H					= 0x23,
	DYNAMIXEL_R_PRESENT_POSITION_L			= 0x24,
	DYNAMIXEL_R_PRESENT_POSITION_H			= 0x25,
	DYNAMIXEL_R_PRESENT_SPEED_L					= 0x26,
	DYNAMIXEL_R_PRESENT_SPEED_H					= 0x27,
	DYNAMIXEL_R_PRESENT_LOAD_L					= 0x28,
	DYNAMIXEL_R_PRESENT_LOAD_H					= 0x29,
	DYNAMIXEL_R_PRESENT_VOLTAGE					= 0x2A,
	DYNAMIXEL_R_PRESENT_TEMPERATURE			= 0x2B,
	DYNAMIXEL_R_REGISTERED_INSTRUCTION	= 0x2C,
	DYNAMIXEL_R_MOVING									= 0x2E,
	DYNAMIXEL_R_LOCK										= 0x2F,
	DYNAMIXEL_R_PUNCH_L									= 0x30,
	DYNAMIXEL_R_PUNCH_H									= 0x31,
} dynamixel_register_t;

extern const unsigned int libdynamixel_version_major;
extern const unsigned int libdynamixel_version_minor;
extern const unsigned int libdynamixel_version_micro;

typedef struct _dynamixel dynamixel_t;

typedef struct Dynamixel_ID_List {
	uint8_t count;
	uint8_t id[256];
}id_list_t;

typedef enum {
	DYNAMIXEL_ERROR_RECOVERY_NONE					= 0,
	DYNAMIXEL_ERROR_RECOVERY_LINK					= (1<<1),
	DYNAMIXEL_ERROR_RECOVERY_PROTOCOL			= (1<<2),
} dynamixel_error_recovery_mode;

typedef enum {
	/* Request message on the server side */
	MSG_INDICATION,
	/* Request message on the client side */
	MSG_CONFIRMATION
} msg_type_t;

int dynamixel_set_error_recovery(dynamixel_t *ctx, dynamixel_error_recovery_mode error_recovery);

/* dynamixel core funcions */
void dynamixel_get_response_timeout(dynamixel_t *ctx, struct timeval *timeout);
void dynamixel_set_response_timeout(dynamixel_t *ctx, const struct timeval *timeout);

void dynamixel_get_byte_timeout(dynamixel_t *ctx, struct timeval *timeout);
void dynamixel_set_byte_timeout(dynamixel_t *ctx, const struct timeval *timeout);

int8_t dynamixel_get_header_length(dynamixel_t *ctx);

void dynamixel_set_debug(dynamixel_t *ctx,bool);

int8_t dynamixel_connect(dynamixel_t *ctx);
void dynamixel_close(dynamixel_t *);
void dynamixel_free(dynamixel_t *ctx);

/* dynamixel basic functions */
int8_t dynamixel_ping(dynamixel_t *ctx, uint8_t id);
int8_t dynamixel_read_data(
	dynamixel_t *ctx,
	uint8_t id,
	dynamixel_register_t address,
	uint8_t length,
	uint8_t** dst
);
int8_t dynamixel_write_data(
	dynamixel_t *ctx,
	uint8_t id,
	dynamixel_register_t address,
	uint8_t length,
	uint8_t* data
);
int8_t dynamixel_reg_write(
	dynamixel_t *ctx,
	uint8_t id,
	dynamixel_register_t address,
	uint8_t length,
	uint8_t* data
);
int8_t dynamixel_action(dynamixel_t *ctx, uint8_t id);
int8_t dynamixel_reset(dynamixel_t *ctx, uint8_t id);

int8_t dynamixel_sync_write(
	dynamixel_t *ctx,
	dynamixel_register_t address,
	uint8_t id_count,
	uint8_t parameter_count,
	uint8_t* data
);

/* combined dynamixel functions */
int8_t dynamixel_reg_write_byte(dynamixel_t *ctx, uint8_t id, dynamixel_register_t address, uint8_t data);
int8_t dynamixel_reg_write_word(dynamixel_t *ctx, uint8_t id, dynamixel_register_t address, uint16_t data);



/* dynamixel abstract functions */
int8_t dynamixel_search(dynamixel_t *ctx, uint8_t start,uint8_t end, uint8_t** dst);

int8_t dynamixel_sync_write_words(
	dynamixel_t *ctx,
	dynamixel_register_t address,
	uint8_t id_count,
	uint8_t word_count,
	uint16_t* data
);


/* advanced non dynamixel commands */
#ifdef TROSSEN_CMD_SUPPORT
typedef struct Trossen_Commander_Data{
	int8_t	right_V;
	int8_t	right_H;
	int8_t	left_V;
	int8_t	left_H;
	uint8_t	buttons;
} trossen_cmd_t;

int8_t dynamixel_adv_trossen_cmd(dynamixel_t*, trossen_cmd_t*);

#endif

DYNAMIXEL_END_DECLS

#endif  /* _DYNAMIXEL_H_ */
