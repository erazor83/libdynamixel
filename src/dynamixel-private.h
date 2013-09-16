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


#ifndef _DYNAMIXEL_PRIVATE_H_
#define _DYNAMIXEL_PRIVATE_H_

#ifndef _MSC_VER
# include <stdint.h>
# include <sys/time.h>
#else
# include "stdint.h"
# include <time.h>
typedef int ssize_t;
#endif
#include <sys/types.h>

#include "dynamixel.h"

DYNAMIXEL_BEGIN_DECLS

#define _MIN_REQ_LENGTH 7


/* Timeouts in microsecond (0.5 s) */
#define _RESPONSE_TIMEOUT_SEARCH	100000
#define _RESPONSE_TIMEOUT					500000
#define _BYTE_TIMEOUT							500000

/* request types */
typedef enum {
	DYNAMIXEL_RQ_PING					=0x01,
	DYNAMIXEL_RQ_READ_DATA		=0x02,
	DYNAMIXEL_RQ_WRITE_DATA		=0x03,
	DYNAMIXEL_RQ_REG_WRITE		=0x04,
	DYNAMIXEL_RQ_REG_ACTION		=0x05,
	DYNAMIXEL_RQ_RESET				=0x06,
	DYNAMIXEL_RQ_SYNC_WRITE		=0x83,
} dynamixel_request_t;

typedef enum {
	_DYNAMIXEL_BACKEND_TYPE_RTU=0
} dynamixel_bakend_type_t;

/* This structure reduces the number of params in functions and so
* optimizes the speed of execution (~ 37%). */
typedef struct _sft {
	int dynamixel_id;
	int instruction;
	int t_id;
} sft_t;

typedef struct _dynamixel_backend {
	uint8_t backend_type;
	uint8_t header_length;
	uint8_t checksum_length;
	uint8_t max_adu_length;
		
	int8_t (*build_request_basis) (dynamixel_t *ctx, uint8_t id, dynamixel_request_t instruction,
																 uint8_t parameter_count, uint8_t *req);
	//int8_t (*build_response_basis) (sft_t *sft, uint8_t *rsp);
	//int8_t (*prepare_response_tid) (const uint8_t *req, int *req_length);
		
	int8_t (*send_msg_pre) (uint8_t *req, uint8_t req_length);
	ssize_t (*send) (dynamixel_t *ctx, const uint8_t *req, uint8_t req_length);
	ssize_t (*recv) (dynamixel_t *ctx, uint8_t *rsp, uint8_t rsp_length);
	int8_t (*check_integrity) (dynamixel_t *ctx, uint8_t *msg,
														const uint8_t msg_length);
	int8_t (*connect) (dynamixel_t *ctx);
	void (*close) (dynamixel_t *ctx);
	int8_t (*flush) (dynamixel_t *ctx);
	int8_t (*select) (dynamixel_t *ctx, fd_set *rfds, struct timeval *tv, uint8_t msg_length);
} dynamixel_backend_t;

struct _dynamixel {
	/* Socket or file descriptor */
	int s;
	bool debug;
	bool error_recovery;
	struct timeval response_timeout;
	struct timeval byte_timeout;
	const dynamixel_backend_t *backend;
	void *backend_data;
	uint8_t *response_data;
};

void _dynamixel_init_common(dynamixel_t *ctx);
void _error_print(dynamixel_t *ctx, const char *context);

#ifndef HAVE_STRLCPY
size_t strlcpy(char *dest, const char *src, size_t dest_size);
#endif

DYNAMIXEL_END_DECLS

#endif  /* _DYNAMIXEL_PRIVATE_H_ */
