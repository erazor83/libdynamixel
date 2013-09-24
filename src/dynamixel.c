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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <limits.h>
#include <time.h>

#include "compat.c"

#include "dynamixel.h"
#include "dynamixel-private.h"

/* Internal use */
#define MSG_LENGTH_UNDEFINED -1

/* Exported version */
const unsigned int libdynamixel_version_major = LIBDYNAMIXEL_VERSION_MAJOR;
const unsigned int libdynamixel_version_minor = LIBDYNAMIXEL_VERSION_MINOR;
const unsigned int libdynamixel_version_micro = LIBDYNAMIXEL_VERSION_MICRO;

/* Max between RTU and TCP max adu length (so TCP) */
#define MAX_MESSAGE_LENGTH 260

/* 3 steps are used to parse the query */
typedef enum {
		_STEP_FUNCTION,
		_STEP_META,
		_STEP_DATA
} _step_t;

const char *dynamixel_strerror(uint8_t errnum) {
	switch (errnum) {
		case ETIMEDOUT:
			return "Timeout";
		case EBADF:
			return "Bad filepointer";
		case ECONNRESET:
			return "Connection reset";
		case ECONNREFUSED:
			return "Connection refused";
		case E_DYNAMIXEL_BADDATA:
			return "Dynamixel bad data";
		default:
			return "Unknown error";
	}
}


int8_t _response_malloc(dynamixel_t *ctx, uint8_t size) {
	if (ctx->response_data) {
		free(ctx->response_data);
	}
	ctx->response_data=(uint8_t*)malloc(size);
	return size;
}

void _error_print(dynamixel_t *ctx, const char *context) {
	if (ctx->debug) {
		fprintf(stderr, "ERROR %s", dynamixel_strerror(errno));
		if (context != NULL) {
			fprintf(stderr, ": %s\n", context);
		} else {
			fprintf(stderr, "\n");
		}
	}
}

int8_t dynamixel_flush(dynamixel_t *ctx) {
	int rc = ctx->backend->flush(ctx);
	if (rc != -1 && ctx->debug) {
		printf("%d bytes flushed\n", rc);
	}
	return rc;
}

int8_t _sleep_and_flush(dynamixel_t *ctx) {
#ifdef _WIN32
	/* usleep doesn't exist on Windows */
	Sleep(
		(ctx->response_timeout.tv_sec * 1000) +
			(ctx->response_timeout.tv_usec / 1000)
	);
#else
	/* usleep source code */
	struct timespec request, remaining;
	request.tv_sec = ctx->response_timeout.tv_sec;
	request.tv_nsec = ((long int)ctx->response_timeout.tv_usec % 1000000)* 1000;
	while (nanosleep(&request, &remaining) == -1 && errno == EINTR) {
		request = remaining;
	}
#endif
	return dynamixel_flush(ctx);
}

#if 0
/* Computes the length of the expected response */
static uint8_t compute_response_length_from_request(dynamixel_t *ctx, uint8_t *req) {
	const uint8_t offset = ctx->backend->header_length;
	uint8_t length=3+req[ctx->backend->header_length+1];

	return offset + length + ctx->backend->checksum_length;
}
#endif

/* Sends a request/response */
static int send_msg(dynamixel_t *ctx, uint8_t *msg, int msg_length) {
	int rc;
	int i;

	msg_length = ctx->backend->send_msg_pre(msg, msg_length);

	if (ctx->debug) {
		for (i = 0; i < msg_length; i++){
			printf("[%.2X]", msg[i]);
		}
		printf("\n");
	}

	/* In recovery mode, the write command will be issued until to be
	 * successful! Disabled by default.
	 */
	do {
		rc = ctx->backend->send(ctx, msg, msg_length);
		if (rc == -1) {
			_error_print(ctx, NULL);
			if (ctx->error_recovery & DYNAMIXEL_ERROR_RECOVERY_LINK) {
				int saved_errno = errno;

				if ((errno == EBADF || errno == ECONNRESET || errno == EPIPE)) {
					dynamixel_close(ctx);
					dynamixel_connect(ctx);
				} else {
					_sleep_and_flush(ctx);
				}
				errno = saved_errno;
			}
		}
	} while ((ctx->error_recovery & DYNAMIXEL_ERROR_RECOVERY_LINK) &&
						rc == -1);

	if (rc > 0 && rc != msg_length) {
		errno = E_DYNAMIXEL_MDATA;
		return -1;
	}

	return rc;
}

int8_t dynamixel_send_raw_request(dynamixel_t *ctx, uint8_t *raw_req, int raw_req_length) {
	//TODO
	return 0;
}

static int receive_msg(dynamixel_t *ctx, uint8_t *msg, msg_type_t msg_type) {
	int rc;
	fd_set rfds;
	struct timeval tv;
	struct timeval *p_tv;
	int length_to_read;
	int msg_length = 0;
	bool frame_start;
	
	if (ctx->debug) {
		if (msg_type == MSG_INDICATION) {
			printf("Waiting for a indication...\n");
		} else {
			printf("Waiting for a confirmation...\n");
		}
	}

	/* Add a file descriptor to the set */
	FD_ZERO(&rfds);
	FD_SET(ctx->s, &rfds);

	/* We need to analyse the message step by step.  At the first step, we want
	* to reach the function code because all packets contain this
	* information. */
	frame_start=true;
	length_to_read = ctx->backend->header_length + 2;

	if (msg_type == MSG_INDICATION) {
		/* Wait for a message, we don't know when the message will be
		 * received 
		 */
		p_tv = NULL;
	} else {
		tv.tv_sec = ctx->response_timeout.tv_sec;
		tv.tv_usec = ctx->response_timeout.tv_usec;
		p_tv = &tv;
	}

	while (length_to_read != 0) {
		rc = ctx->backend->select(ctx, &rfds, p_tv, length_to_read);
		if (rc == -1) {
			_error_print(ctx, "select");
			if (ctx->error_recovery & DYNAMIXEL_ERROR_RECOVERY_LINK) {
				int saved_errno = errno;

				if (errno == ETIMEDOUT) {
					_sleep_and_flush(ctx);
				} else if (errno == EBADF) {
					dynamixel_close(ctx);
					dynamixel_connect(ctx);
				}
				errno = saved_errno;
			}
			return -1;
		}

		rc = ctx->backend->recv(ctx, msg + msg_length, length_to_read);
		if (rc == 0) {
			errno = ECONNRESET;
			rc = -1;
		}
		if (rc == -1) {
			_error_print(ctx, "read");
			if ((ctx->error_recovery & DYNAMIXEL_ERROR_RECOVERY_LINK) &&
					((errno == ECONNRESET) || (errno == ECONNREFUSED) ||
					(errno == EBADF))) {
				int saved_errno = errno;
				dynamixel_close(ctx);
				dynamixel_connect(ctx);
				/* Could be removed by previous calls */
				errno = saved_errno;
			}
			return -1;
		}

		/* Display the hex code of each character received */
		if (ctx->debug) {
			int i;
			for (i=0; i < rc; i++) {
				printf("<%.2X>", msg[msg_length + i]);
			}
		}

		/* Sums bytes received */
		msg_length += rc;
		/* Computes remaining bytes */
		length_to_read -= rc;

		if (length_to_read == 0) {
			if (frame_start) {
				//check for 0xff / 0xff and min length
				if ((msg[0]==0xff) && (msg[1]==0xff)) {
					length_to_read=msg[3];
					frame_start=false;
				} else {
					errno=E_DYNAMIXEL_BADDATA;
					_error_print(ctx, "invalid frame start");
				}
			}
		}

		if ((length_to_read>0) && (ctx->byte_timeout.tv_sec != -1)) {
			/* If there is no character in the buffer, the allowed timeout
			* interval between two consecutive bytes is defined by
			* byte_timeout
			*/
			tv.tv_sec = ctx->byte_timeout.tv_sec;
			tv.tv_usec = ctx->byte_timeout.tv_usec;
			p_tv = &tv;
		}
	}

	if (ctx->debug) {
		printf("\n");
	}
				
	/* check_integrity will remove header and checksum from message */
	/* returning message structure:
	 * 0xFF 0xFF ID LENGTH INSTRUCTION PARAMETERS CHECKSUM
	 * ID LENGTH INSTRUCTION PARAMETERS
	 */

	return ctx->backend->check_integrity(ctx, msg, msg_length);
}

/* Receives the confirmation.

	The function shall store the read response in rsp and return the number of
	values (bits or words). Otherwise, its shall return -1 and errno is set.

	The function doesn't check the confirmation is the expected response to the
	initial request.
*/
int dynamixel_receive_confirmation(dynamixel_t *ctx, uint8_t *rsp) {
	return receive_msg(ctx, rsp, MSG_CONFIRMATION);
}

void _dynamixel_init_common(dynamixel_t *ctx) {
	/* Slave and socket are initialized to -1 */
	ctx->s = -1;

	ctx->debug = false;
	ctx->error_recovery = DYNAMIXEL_ERROR_RECOVERY_NONE;

	ctx->response_timeout.tv_sec = 0;
	ctx->response_timeout.tv_usec = _RESPONSE_TIMEOUT;

	ctx->byte_timeout.tv_sec = 0;
	ctx->byte_timeout.tv_usec = _BYTE_TIMEOUT;

	ctx->response_data = NULL;
}


int dynamixel_set_error_recovery(dynamixel_t *ctx,
															dynamixel_error_recovery_mode error_recovery) {
	/* The type of dynamixel_error_recovery_mode is unsigned enum */
	ctx->error_recovery = (uint8_t) error_recovery;
	return 0;
}

void dynamixel_set_socket(dynamixel_t *ctx, int socket) {
	ctx->s = socket;
}

int dynamixel_get_socket(dynamixel_t *ctx) {
	return ctx->s;
}

/* Get the timeout interval used to wait for a response */
void dynamixel_get_response_timeout(dynamixel_t *ctx, struct timeval *timeout) {
	*timeout = ctx->response_timeout;
}

void dynamixel_set_response_timeout(dynamixel_t *ctx, const struct timeval *timeout) {
	ctx->response_timeout = *timeout;
}

/* Get the timeout interval between two consecutive bytes of a message */
void dynamixel_get_byte_timeout(dynamixel_t *ctx, struct timeval *timeout) {
	*timeout = ctx->byte_timeout;
}

void dynamixel_set_byte_timeout(dynamixel_t *ctx, const struct timeval *timeout) {
	ctx->byte_timeout = *timeout;
}

int8_t dynamixel_get_header_length(dynamixel_t *ctx) {
	return ctx->backend->header_length;
}

int8_t dynamixel_connect(dynamixel_t *ctx) {
	return ctx->backend->connect(ctx);
}

void dynamixel_close(dynamixel_t *ctx) {
	if (ctx == NULL) {
		return;
	}

	ctx->backend->close(ctx);
}

void dynamixel_free(dynamixel_t *ctx) {
	if (ctx == NULL) {
		return;
	}

	if (ctx->response_data != NULL) {
		free(ctx->response_data);
	}
	free(ctx->backend_data);
	free(ctx);
}

void dynamixel_set_debug(dynamixel_t *ctx, bool value) {
	ctx->debug = value;
}


int8_t dynamixel_ping(dynamixel_t *ctx, uint8_t id) {
	int8_t rc;
	uint8_t req_length;
	uint8_t req[_MIN_REQ_LENGTH];
	uint8_t rsp[MAX_MESSAGE_LENGTH];
		
	req_length = ctx->backend->build_request_basis(ctx,id, DYNAMIXEL_RQ_PING, 0, req);
	rc = send_msg(ctx, req, req_length);
	if (rc > 0) {
		rc = receive_msg(ctx, rsp, MSG_CONFIRMATION);
		if (rc == -1) {
			if (errno==ETIMEDOUT) {
				return 0;
			} else {
				return -1;
			}
		} else {
			return 1;
		}
	}

	return -1;
}

int8_t dynamixel_read_data(dynamixel_t *ctx, uint8_t id,
													 dynamixel_register_t address, uint8_t length, uint8_t** dst) {
	int8_t rc;
	uint8_t req_length;
	uint8_t req[_MIN_REQ_LENGTH+2];
	uint8_t rsp[MAX_MESSAGE_LENGTH];
		

	req_length = ctx->backend->build_request_basis(ctx,id, DYNAMIXEL_RQ_READ_DATA, 2, req);
	req[req_length++]=address;
	req[req_length++]=length;
	
	rc = send_msg(ctx, req, req_length);
	if (rc > 0) {
		rc = receive_msg(ctx, rsp, MSG_CONFIRMATION);
		if (rc == -1) {
			return rc;
		} else {
			if (rsp[2]!=0) {
				//dynamixel_error
				return -rsp[2];
			} else {
				_response_malloc(ctx,rsp[1]-2);
				memcpy(ctx->response_data,rsp+3,rsp[1]-2);
				*dst=ctx->response_data;
			}
			return rsp[1]-2;
		}
	}

	return -1;
}

int8_t dynamixel_action(dynamixel_t *ctx, uint8_t id) {
	int8_t rc;
	uint8_t req_length;
	uint8_t req[_MIN_REQ_LENGTH];
		
	req_length = ctx->backend->build_request_basis(ctx,id, DYNAMIXEL_RQ_REG_ACTION, 0, req);
	rc = send_msg(ctx, req, req_length);
	
	return rc;
}

int8_t dynamixel_reset(dynamixel_t *ctx, uint8_t id) {
	int8_t rc;
	uint8_t req_length;
	uint8_t req[_MIN_REQ_LENGTH];
		
	req_length = ctx->backend->build_request_basis(ctx,id, DYNAMIXEL_RQ_RESET, 0, req);
	rc = send_msg(ctx, req, req_length);
	
	return rc;
}

int8_t dynamixel_write_data(dynamixel_t *ctx, uint8_t id,
													 dynamixel_register_t address, uint8_t length,uint8_t* data) {
	int8_t rc;
	uint8_t req_length;
	uint8_t *req;
	
	req=malloc(_MIN_REQ_LENGTH+1+length);

	
		
	req_length = ctx->backend->build_request_basis(ctx,id, DYNAMIXEL_RQ_WRITE_DATA, 1+length, req);
	req[req_length++]=address;
	for (rc=0;rc<length;rc++){
		req[req_length++]=data[rc];
	}

	rc = send_msg(ctx, req, req_length);
	return rc;
	
	/* there is no response */
}

int8_t dynamixel_reg_write(dynamixel_t *ctx, uint8_t id,
													 dynamixel_register_t address, uint8_t length,uint8_t* data) {
	int8_t rc;
	uint8_t req_length;
	uint8_t *req;
	
	req=malloc(_MIN_REQ_LENGTH+1+length);
	req_length = ctx->backend->build_request_basis(ctx,id, DYNAMIXEL_RQ_REG_WRITE, 1+length, req);
	req[req_length++]=address;
	for (rc=0;rc<length;rc++){
		req[req_length++]=data[rc];
	}

	rc = send_msg(ctx, req, req_length);
	return rc;
	
	/* there is no response */
}


int8_t dynamixel_search(dynamixel_t *ctx, uint8_t start,uint8_t end, uint8_t** dst) {
	int8_t ret=0;
	int8_t ping;
	uint8_t cid;
	
	ctx->response_timeout.tv_sec = 0;
	ctx->response_timeout.tv_usec = _RESPONSE_TIMEOUT_SEARCH;

	_response_malloc(ctx,end-start+1);

	for (cid=start;cid<=end;cid++) {
		if (ctx->debug) {
			fprintf(stderr, "ping % 3i ...",cid);
		}
		ping=dynamixel_ping(ctx,cid);
		if (ping==1) {
			ctx->response_data[ret]=cid;
			ret++;
			if (ctx->debug) {
				printf(" OK\n");
			}
		} else if (ping==0) {
			if (ctx->debug) {
				printf(" TIMEOUT\n");
			}
		} else {
			if (ctx->debug) {
				printf(" ERROR\n");
			}
		}
	}
	*dst=ctx->response_data;
	
	ctx->response_timeout.tv_sec = 0;
	ctx->response_timeout.tv_usec = _RESPONSE_TIMEOUT;
	return ret;
}

int8_t dynamixel_reg_write_byte(dynamixel_t *ctx, uint8_t id, dynamixel_register_t address, uint8_t data) {
	//UNTESTED
	int8_t rc;
	uint8_t req_length;
	uint8_t req[_MIN_REQ_LENGTH+2];
	req_length = ctx->backend->build_request_basis(ctx,id, DYNAMIXEL_RQ_WRITE_DATA, 2, req);
	req[req_length++]=address;
	req[req_length++]=data;
	rc = send_msg(ctx, req, req_length);
	return rc;
}
int8_t dynamixel_reg_write_word(dynamixel_t *ctx, uint8_t id, dynamixel_register_t address, uint16_t data) {
	//UNTESTED
	int8_t rc;
	uint8_t req_length;
	uint8_t req[_MIN_REQ_LENGTH+3];
	req_length = ctx->backend->build_request_basis(ctx,id, DYNAMIXEL_RQ_WRITE_DATA, 3, req);
	req[req_length++]=address;
	req[req_length++]=data&0xff;
	req[req_length++]=(data>>8)&0xff;
	rc = send_msg(ctx, req, req_length);
	return rc;
}
int8_t dynamixel_sync_write(
	dynamixel_t *ctx,
	dynamixel_register_t address,
	uint8_t id_count,
	uint8_t parameter_count,
	uint8_t* data
) {
	/*
	 * data:
	 *   <dynamixel_id>
	 *   <parameter>
	 *   <parameter>
	 *   <dynamixel_id>
	 *   <parameter>
	 *   <parameter>
	 */
	int8_t rc;
	uint8_t req_length;
	uint8_t *req;
	
	uint8_t cID;
	uint8_t cP;
	
	req=malloc(_MIN_REQ_LENGTH+2+(parameter_count+1)*id_count);
	req_length = ctx->backend->build_request_basis(
		ctx,
		DYNAMIXEL_BROADCAST_ADDRESS,
		DYNAMIXEL_RQ_SYNC_WRITE,
		2+(parameter_count+1)*id_count,
		req
	);
	/* starting address */
	req[req_length++]=address;
	
	/* count of parameters */
	req[req_length++]=parameter_count;
	
	for (cID=0;cID<id_count;cID++) {
		/* servo id */
		req[req_length++]=*(data++);
		/* parameters */
		for (cP=0;cP<parameter_count;cP++) {
			req[req_length++]=*(data++);
		}
	}
	
	rc = send_msg(ctx, req, req_length);
	return rc;
}

int8_t dynamixel_sync_write_words(
	dynamixel_t *ctx,
	dynamixel_register_t address,
	uint8_t id_count,
	uint8_t word_count,
	uint16_t* data
) {

	int8_t rc;
	uint8_t req_length;
	uint8_t *req;
	
	uint8_t cID;
	uint8_t cW;
	uint16_t cP;
	
	
	req=malloc(_MIN_REQ_LENGTH+2+(word_count*2+1)*id_count);
	req_length = ctx->backend->build_request_basis(
		ctx,
		DYNAMIXEL_BROADCAST_ADDRESS,
		DYNAMIXEL_RQ_SYNC_WRITE,
		2+(word_count*2+1)*id_count,
		req
	);
	/* starting address */
	req[req_length++]=address;
	
	/* count of parameters */
	req[req_length++]=word_count*2;
	
	for (cID=0;cID<id_count;cID++) {
		/* servo id */
		req[req_length++]=data[cID*(word_count+1)]&0xff;
		/* words */
		for (cW=1;cW<=word_count;cW++) {
			cP=data[cID*(word_count+1)+cW];
			req[req_length++]=cP&0xff;
			req[req_length++]=cP>>8;
		}
	}
	
	rc = send_msg(ctx, req, req_length);
	return rc;
}



