/*
* Copyright © 2001-2011 Ste´phane Raimbault <stephane.raimbault@gmail.com>
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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#ifndef _MSC_VER
#include <unistd.h>
#endif
#include <assert.h>

#include "dynamixel-private.h"

#include "dynamixel-rtu.h"
#include "dynamixel-rtu-private.h"

#if HAVE_DECL_TIOCSRS485
#include <sys/ioctl.h>
#include <linux/serial.h>
#endif

uint8_t gen_checksum(uint8_t* req, uint8_t req_length) {
	uint16_t ret;
	uint8_t w;
	for (w=2; w<req_length; w++) {
		ret=ret+req[w];
	}
	return (~ret)&0xff;
}

/* Builds a RTU request header */
static int8_t _dynamixel_rtu_build_request_basis(dynamixel_t *ctx, uint8_t id, uint8_t parameter_count,
																dynamixel_request_t instruction, uint8_t *req) {
	req[0] = 0xff;
	req[1] = 0xff;
	req[2] = id;
	req[3] = parameter_count+2;
	req[4] = instruction;

	return _DYNAMIXEL_RTU_PRESET_REQ_LENGTH;
}

int8_t _dynamixel_rtu_send_msg_pre(uint8_t *req, uint8_t req_length) {
	uint8_t checksum = gen_checksum(req, req_length);
	req[req_length++] = checksum;

	return req_length;
}

#if defined(_WIN32)

/* This simple implementation is sort of a substitute of the select() call,
 * working this way: the win32_ser_select() call tries to read some data from
 * the serial port, setting the timeout as the select() call would. Data read is
 * stored into the receive buffer, that is then consumed by the win32_ser_read()
 * call.  So win32_ser_select() does both the event waiting and the reading,
 * while win32_ser_read() only consumes the receive buffer.
 */

static void win32_ser_init(struct win32_ser *ws) {
	/* Clear everything */
	memset(ws, 0x00, sizeof(struct win32_ser));

	/* Set file handle to invalid */
	ws->fd = INVALID_HANDLE_VALUE;
}

/* FIXME Try to remove length_to_read -> max_len argument, only used by win32 */
static int8_t win32_ser_select(struct win32_ser *ws, uint8_t max_len,
																struct timeval *tv) {
	COMMTIMEOUTS comm_to;
	unsigned int msec = 0;

	/* Check if some data still in the buffer to be consumed */
	if (ws->n_bytes > 0) {
		return 1;
	}

	/* Setup timeouts like select() would do.
	 * FIXME Please someone on Windows can look at this?
	 * Does it possible to use WaitCommEvent?
	 * When tv is NULL, MAXDWORD isn't infinite!
	 */
	if (tv == NULL) {
		msec = MAXDWORD;
	} else {
		msec = tv->tv_sec * 1000 + tv->tv_usec / 1000;
		if (msec < 1) {
			msec = 1;
		}
	}

	comm_to.ReadIntervalTimeout 				= msec;
	comm_to.ReadTotalTimeoutMultiplier 	= 0;
	comm_to.ReadTotalTimeoutConstant 		= msec;
	comm_to.WriteTotalTimeoutMultiplier	= 0;
	comm_to.WriteTotalTimeoutConstant 	= 1000;
	SetCommTimeouts(ws->fd, &comm_to);

	/* Read some bytes */
	if ((max_len > PY_BUF_SIZE) || (max_len < 0)) {
		max_len = PY_BUF_SIZE;
	}

	if (ReadFile(ws->fd, &ws->buf, max_len, &ws->n_bytes, NULL)) {
		/* Check if some bytes available */
		if (ws->n_bytes > 0) {
			/* Some bytes read */
			return 1;
		} else {
			/* Just timed out */
			return 0;
		}
	} else {
		/* Some kind of error */
		return -1;
	}
}

static int8_t win32_ser_read(struct win32_ser *ws, uint8_t *p_msg,
															uint8_t max_len) {
	uint8_t n = ws->n_bytes;

	if (max_len < n) {
		n = max_len;
	}

	if (n > 0) {
		memcpy(p_msg, ws->buf, n);
	}

	ws->n_bytes -= n;

	return n;
}
#endif

ssize_t _dynamixel_rtu_send(dynamixel_t *ctx, const uint8_t *req, uint8_t req_length) {
#if defined(_WIN32)
	dynamixel_rtu_t *ctx_rtu = ctx->backend_data;
	DWORD n_bytes = 0;
	return (WriteFile(ctx_rtu->w_ser.fd, req, req_length, &n_bytes, NULL)) ? n_bytes : -1;
#else
	return write(ctx->s, req, req_length);
#endif
}

ssize_t _dynamixel_rtu_recv(dynamixel_t *ctx, uint8_t *rsp, uint8_t rsp_length) {
#if defined(_WIN32)
	return win32_ser_read(&((dynamixel_rtu_t *)ctx->backend_data)->w_ser, rsp, rsp_length);
#else
	return read(ctx->s, rsp, rsp_length);
#endif
}

int8_t _dynamixel_rtu_flush(dynamixel_t *);

int8_t _dynamixel_rtu_check_integrity(dynamixel_t *ctx, uint8_t *msg,
																			uint8_t msg_length) {
	uint8_t checksum_calculated;
	uint8_t checksum_received;

	checksum_calculated = gen_checksum(msg, msg_length - 1);
	checksum_received = msg[msg_length - 1];

	/* Check Checksum of msg */
	if (checksum_calculated == checksum_received) {
		memmove(
			msg,
			msg+_DYNAMIXEL_RTU_HEADER_LENGTH,
			msg_length-_DYNAMIXEL_RTU_HEADER_LENGTH-_DYNAMIXEL_RTU_CHECKSUM_LENGTH
		);
		msg_length=msg_length-_DYNAMIXEL_RTU_HEADER_LENGTH-_DYNAMIXEL_RTU_CHECKSUM_LENGTH;
		return msg_length;
	} else {
		if (ctx->debug) {
			fprintf(stderr, "ERROR Checksum received %0X != Checksum calculated %0X\n",
							checksum_received, checksum_calculated);
		}
		if (ctx->error_recovery & DYNAMIXEL_ERROR_RECOVERY_PROTOCOL) {
			_dynamixel_rtu_flush(ctx);
		}
		errno = E_DYNAMIXEL_BADCHECKSUM;
		return -1;
	}
}

/* Sets up a serial port for RTU communications */
static int8_t _dynamixel_rtu_connect(dynamixel_t *ctx) {
#if defined(_WIN32)
	DCB dcb;
#else
	struct termios tios;
	speed_t speed;
#endif
	dynamixel_rtu_t *ctx_rtu = ctx->backend_data;

	if (ctx->debug) {
		printf(
			"Opening %s at %d bauds (%c, %d, %d)\n",
			ctx_rtu->device, ctx_rtu->baud, ctx_rtu->parity,
			ctx_rtu->data_bit, ctx_rtu->stop_bit
		);
	}

#if defined(_WIN32)
	/* Some references here:
	 * http://msdn.microsoft.com/en-us/library/aa450602.aspx
	 */
	win32_ser_init(&ctx_rtu->w_ser);

	/* ctx_rtu->device should contain a string like "COMxx:" xx being a decimal
	 * number
	 */
	ctx_rtu->w_ser.fd = CreateFileA(
		ctx_rtu->device,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL
	);

	/* Error checking */
	if (ctx_rtu->w_ser.fd == INVALID_HANDLE_VALUE) {
		fprintf(stderr, "ERROR Can't open the device %s (%s)\n",
						ctx_rtu->device, strerror(errno));
		return -1;
	}

	/* Save params */
	ctx_rtu->old_dcb.DCBlength = sizeof(DCB);
	if (!GetCommState(ctx_rtu->w_ser.fd, &ctx_rtu->old_dcb)) {
		fprintf(
			stderr,
			"ERROR Error getting configuration (LastError %d)\n",
			(int)GetLastError()
		);
		CloseHandle(ctx_rtu->w_ser.fd);
		ctx_rtu->w_ser.fd = INVALID_HANDLE_VALUE;
		return -1;
	}

	/* Build new configuration (starting from current settings) */
	dcb = ctx_rtu->old_dcb;

	/* Speed setting */
	switch (ctx_rtu->baud) {
		case 110:
			dcb.BaudRate = CBR_110;
			break;
		case 300:
			dcb.BaudRate = CBR_300;
			break;
		case 600:
			dcb.BaudRate = CBR_600;
			break;
		case 1200:
			dcb.BaudRate = CBR_1200;
			break;
		case 2400:
			dcb.BaudRate = CBR_2400;
			break;
		case 4800:
			dcb.BaudRate = CBR_4800;
			break;
		case 9600:
			dcb.BaudRate = CBR_9600;
			break;
		case 19200:
			dcb.BaudRate = CBR_19200;
			break;
		case 38400:
			dcb.BaudRate = CBR_38400;
			break;
		case 57600:
			dcb.BaudRate = CBR_57600;
			break;
		case 115200:
			dcb.BaudRate = CBR_115200;
			break;
#ifdef HAVE_BAUDRATE_200000
		case 200000:
			dcb.BaudRate = CBR_200000;
			break;
#endif
#ifdef HAVE_BAUDRATE_250000
		case 250000:
			dcb.BaudRate = CBR_250000;
			break;
#endif
#ifdef HAVE_BAUDRATE_400000
		case 400000:
			dcb.BaudRate = CBR_400000;
			break;
#endif
#ifdef HAVE_BAUDRATE_500000
		case 500000:
			dcb.BaudRate = CBR_500000;
			break;
#endif
#ifdef HAVE_BAUDRATE_1000000
		case 1000000:
			dcb.BaudRate = CBR_1000000;
			break;
#endif
		default:
			dcb.BaudRate = CBR_9600;
			printf("WARNING Unknown baud rate %d for %s (B9600 used)\n",
							ctx_rtu->baud, ctx_rtu->device);
	}

	/* Data bits */
	switch (ctx_rtu->data_bit) {
		case 5:
			dcb.ByteSize = 5;
			break;
		case 6:
			dcb.ByteSize = 6;
			break;
		case 7:
			dcb.ByteSize = 7;
			break;
		case 8:
		default:
			dcb.ByteSize = 8;
			break;
	}

	/* Stop bits */
	if (ctx_rtu->stop_bit == 1) {
		dcb.StopBits = ONESTOPBIT;
	} else { /* 2 */
		dcb.StopBits = TWOSTOPBITS;
	}
	
	/* Parity */
	if (ctx_rtu->parity == 'N') {
		dcb.Parity = NOPARITY;
		dcb.fParity = FALSE;
	} else if (ctx_rtu->parity == 'E') {
		dcb.Parity = EVENPARITY;
		dcb.fParity = TRUE;
	} else {
		/* odd */
		dcb.Parity = ODDPARITY;
		dcb.fParity = TRUE;
	}

	/* Hardware handshaking left as default settings retrieved */

	/* No software handshaking */
	dcb.fTXContinueOnXoff = TRUE;
	dcb.fOutX = FALSE;
	dcb.fInX = FALSE;

	/* Binary mode (it's the only supported on Windows anyway) */
	dcb.fBinary = TRUE;

	/* Don't want errors to be blocking */
	dcb.fAbortOnError = FALSE;

	/* TODO: any other flags!? */

	/* Setup port */
	if (!SetCommState(ctx_rtu->w_ser.fd, &dcb)) {
		fprintf(
			stderr,
			"ERROR Error setting new configuration (LastError %d)\n",
			(int)GetLastError()
		);
		CloseHandle(ctx_rtu->w_ser.fd);
		ctx_rtu->w_ser.fd = INVALID_HANDLE_VALUE;
		return -1;
	}
#else
	/* The O_NOCTTY flag tells UNIX that this program doesn't want
	 * to be the "controlling terminal" for that port. If you
	 * don't specify this then any input (such as keyboard abort
	 * signals and so forth) will affect your process
	 * 
	 * Timeouts are ignored in canonical input mode or when the
	 * NDELAY option is set on the file via open or fcntl
	 */
	ctx->s = open(ctx_rtu->device, O_RDWR | O_NOCTTY | O_NDELAY | O_EXCL);
	if (ctx->s == -1) {
		fprintf(
			stderr,
			"ERROR Can't open the device %s (%s)\n",
			ctx_rtu->device, strerror(errno)
		);
		return -1;
	}

	/* Save */
	tcgetattr(ctx->s, &(ctx_rtu->old_tios));

	memset(&tios, 0, sizeof(struct termios));

	/*
	 * C_ISPEED     Input baud (new interface)
	 * C_OSPEED     Output baud (new interface)
	*/
	switch (ctx_rtu->baud) {
		case 110:
			speed = B110;
			break;
		case 300:
			speed = B300;
			break;
		case 600:
			speed = B600;
			break;
		case 1200:
			speed = B1200;
			break;
		case 2400:
			speed = B2400;
			break;
		case 4800:
			speed = B4800;
			break;
		case 9600:
			speed = B9600;
			break;
		case 19200:
			speed = B19200;
			break;
		case 38400:
			speed = B38400;
			break;
		case 57600:
			speed = B57600;
			break;
		case 115200:
			speed = B115200;
			break;
#ifdef HAVE_BAUDRATE_200000
		case 200000:
			speed = B200000;
			break;
#endif
#ifdef HAVE_BAUDRATE_250000
		case 250000:
			speed = B250000;
			break;
#endif
#ifdef HAVE_BAUDRATE_400000
		case 400000:
			speed = B400000;
			break;
#endif
#ifdef HAVE_BAUDRATE_500000
		case 500000:
			speed = B500000;
			break;
#endif
#ifdef HAVE_BAUDRATE_1000000
		case 1000000:
			speed = B1000000;
			break;
#endif
		default:
			speed = B9600;
			if (ctx->debug) {
				fprintf(
					stderr,
					"WARNING Unknown baud rate %d for %s (B9600 used)\n",
					ctx_rtu->baud,
					ctx_rtu->device
				);
			}
		}

		/* Set the baud rate */
		if ((cfsetispeed(&tios, speed) < 0) ||
				(cfsetospeed(&tios, speed) < 0)) {
			close(ctx->s);
			ctx->s = -1;
			return -1;
	}

	/*
	 * C_CFLAG      Control options
	 * CLOCAL       Local line - do not change "owner" of port
	 * CREAD        Enable receiver
	 */
	tios.c_cflag |= (CREAD | CLOCAL);
	/* CSIZE, HUPCL, CRTSCTS (hardware flow control) */

	/* Set data bits (5, 6, 7, 8 bits)
	 * CSIZE        Bit mask for data bits
	 */
	tios.c_cflag &= ~CSIZE;
	switch (ctx_rtu->data_bit) {
		case 5:
			tios.c_cflag |= CS5;
			break;
		case 6:
			tios.c_cflag |= CS6;
			break;
		case 7:
			tios.c_cflag |= CS7;
			break;
		case 8:
		default:
			tios.c_cflag |= CS8;
			break;
	}

	/* Stop bit (1 or 2) */
	if (ctx_rtu->stop_bit == 1) {
		tios.c_cflag &=~ CSTOPB;
	} else { /* 2 */
		tios.c_cflag |= CSTOPB;
	}
	/* PARENB       Enable parity bit
	 * PARODD       Use odd parity instead of even
	 */
	if (ctx_rtu->parity == 'N') {
		/* None */
		tios.c_cflag &=~ PARENB;
	} else if (ctx_rtu->parity == 'E') {
		/* Even */
		tios.c_cflag |= PARENB;
		tios.c_cflag &=~ PARODD;
	} else {
		/* Odd */
		tios.c_cflag |= PARENB;
		tios.c_cflag |= PARODD;
	}

	/* Read the man page of termios if you need more information. */

	/* This field isn't used on POSIX systems
	 * tios.c_line = 0;
	 */

	/*
	 * C_LFLAG      Line options
	 * 
	 * ISIG Enable SIGINTR, SIGSUSP, SIGDSUSP, and SIGQUIT signals
	 * ICANON       Enable canonical input (else raw)
	 * XCASE        Map uppercase \lowercase (obsolete)
	 * ECHO Enable echoing of input characters
	 * ECHOE        Echo erase character as BS-SP-BS
	 * ECHOK        Echo NL after kill character
	 * ECHONL       Echo NL
	 * NOFLSH       Disable flushing of input buffers after
	 * interrupt or quit characters
	 * IEXTEN       Enable extended functions
	 * ECHOCTL      Echo control characters as ^char and delete as ~?
	 * ECHOPRT      Echo erased character as character erased
	 * ECHOKE       BS-SP-BS entire line on line kill
	 * FLUSHO       Output being flushed
	 * PENDIN       Retype pending input at next read or input char
	 * TOSTOP       Send SIGTTOU for background output
	 * 
	 * Canonical input is line-oriented. Input characters are put
	 * into a buffer which can be edited interactively by the user
	 * until a CR (carriage return) or LF (line feed) character is
	 * received.
	 * 
	 * Raw input is unprocessed. Input characters are passed
	 * through exactly as they are received, when they are
	 * received. Generally you'll deselect the ICANON, ECHO,
	 * ECHOE, and ISIG options when using raw input
	 */

	/* Raw input */
	tios.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

	/* C_IFLAG      Input options
	 * 
	 * Constant     Description
	 * INPCK        Enable parity check
	 * IGNPAR       Ignore parity errors
	 * PARMRK       Mark parity errors
	 * ISTRIP       Strip parity bits
	 * IXON Enable software flow control (outgoing)
	 * IXOFF        Enable software flow control (incoming)
	 * IXANY        Allow any character to start flow again
	 * IGNBRK       Ignore break condition
	 * BRKINT       Send a SIGINT when a break condition is detected
	 * INLCR        Map NL to CR
	 * IGNCR        Ignore CR
	 * ICRNL        Map CR to NL
	 * IUCLC        Map uppercase to lowercase
	 * IMAXBEL      Echo BEL on input line too long
	 */
	if (ctx_rtu->parity == 'N') {
		/* None */
		tios.c_iflag &= ~INPCK;
	} else {
		tios.c_iflag |= INPCK;
	}

	/* Software flow control is disabled */
	tios.c_iflag &= ~(IXON | IXOFF | IXANY);

	/* C_OFLAG      Output options
	 * OPOST        Postprocess output (not set = raw output)
	 * ONLCR        Map NL to CR-NL
	 * 
	 * ONCLR ant others needs OPOST to be enabled
	 */

	/* Raw ouput */
	tios.c_oflag &=~ OPOST;

	/* C_CC         Control characters
	 * VMIN         Minimum number of characters to read
	 * VTIME        Time to wait for data (tenths of seconds)
	 * 
	 * UNIX serial interface drivers provide the ability to
	 * specify character and packet timeouts. Two elements of the
	 * c_cc array are used for timeouts: VMIN and VTIME. Timeouts
	 * are ignored in canonical input mode or when the NDELAY
	 * option is set on the file via open or fcntl.
	 * 
	 * VMIN specifies the minimum number of characters to read. If
	 * it is set to 0, then the VTIME value specifies the time to
	 * wait for every character read. Note that this does not mean
	 * that a read call for N bytes will wait for N characters to
	 * come in. Rather, the timeout will apply to the first
	 * character and the read call will return the number of
	 * characters immediately available (up to the number you
	 * request).
	 * 
	 * If VMIN is non-zero, VTIME specifies the time to wait for
	 * the first character read. If a character is read within the
	 * time given, any read will block (wait) until all VMIN
	 * characters are read. That is, once the first character is
	 * read, the serial interface driver expects to receive an
	 * entire packet of characters (VMIN bytes total). If no
	 * character is read within the time allowed, then the call to
	 * read returns 0. This method allows you to tell the serial
	 * driver you need exactly N bytes and any read call will
	 * return 0 or N bytes. However, the timeout only applies to
	 * the first character read, so if for some reason the driver
	 * misses one character inside the N byte packet then the read
	 * call could block forever waiting for additional input
	 * characters.
	 * 
	 * VTIME specifies the amount of time to wait for incoming
	 * characters in tenths of seconds. If VTIME is set to 0 (the
	 * default), reads will block (wait) indefinitely unless the
	 * NDELAY option is set on the port with open or fcntl.
	 */
		
	/* Unused because we use open with the NDELAY option */
	tios.c_cc[VMIN] = 0;
	tios.c_cc[VTIME] = 0;

	if (tcsetattr(ctx->s, TCSANOW, &tios) < 0) {
		close(ctx->s);
		ctx->s = -1;
		return -1;
	}
#endif

#if HAVE_DECL_TIOCSRS485
	/* The RS232 mode has been set by default */
	ctx_rtu->serial_mode = DYNAMIXEL_RTU_RS232;
#endif

	return 0;
}

int8_t dynamixel_rtu_set_serial_mode(dynamixel_t *ctx, int8_t mode) {
	if (ctx->backend->backend_type == _DYNAMIXEL_BACKEND_TYPE_RTU) {
#if HAVE_DECL_TIOCSRS485
		dynamixel_rtu_t *ctx_rtu = ctx->backend_data;
		struct serial_rs485 rs485conf;
		memset(&rs485conf, 0x0, sizeof(struct serial_rs485));

		if (mode == DYNAMIXEL_RTU_RS485) {
			rs485conf.flags = SER_RS485_ENABLED;
			if (ioctl(ctx->s, TIOCSRS485, &rs485conf) < 0) {
				return -1;
			}

			ctx_rtu->serial_mode |= DYNAMIXEL_RTU_RS485;
			return 0;
		} else if (mode == DYNAMIXEL_RTU_RS232) {
			if (ioctl(ctx->s, TIOCSRS485, &rs485conf) < 0) {
				return -1;
			}

			ctx_rtu->serial_mode = DYNAMIXEL_RTU_RS232;
			return 0;
		}
#else
		if (ctx->debug) {
			fprintf(stderr, "This function isn't supported on your platform\n");
		}
		errno = ENOTSUP;
	return -1;
#endif
}

	/* Wrong backend and invalid mode specified */
	errno = EINVAL;
	return -1;
}

int8_t dynamixel_rtu_get_serial_mode(dynamixel_t *ctx) {
	if (ctx->backend->backend_type == _DYNAMIXEL_BACKEND_TYPE_RTU) {
#if HAVE_DECL_TIOCSRS485
		dynamixel_rtu_t *ctx_rtu = ctx->backend_data;
		return ctx_rtu->serial_mode;
#else
		if (ctx->debug) {
			fprintf(stderr, "This function isn't supported on your platform\n");
		}
		errno = ENOTSUP;
		return -1;
#endif
	} else {
		errno = EINVAL;
		return -1;
	}
}

void _dynamixel_rtu_close(dynamixel_t *ctx) {
	/* Closes the file descriptor in RTU mode */
	dynamixel_rtu_t *ctx_rtu = ctx->backend_data;

#if defined(_WIN32)
	/* Revert settings */
	if (!SetCommState(ctx_rtu->w_ser.fd, &ctx_rtu->old_dcb)) {
		fprintf(
			stderr,
			"ERROR Couldn't revert to configuration (LastError %d)\n",
			(int)GetLastError()
		);
	}

	if (!CloseHandle(ctx_rtu->w_ser.fd)) {
		fprintf(
			stderr,
			"ERROR Error while closing handle (LastError %d)\n",
			(int)GetLastError()
		);
	}
#else
	tcsetattr(ctx->s, TCSANOW, &(ctx_rtu->old_tios));
	close(ctx->s);
#endif
}

int8_t _dynamixel_rtu_flush(dynamixel_t *ctx) {
#if defined(_WIN32)
	dynamixel_rtu_t *ctx_rtu = ctx->backend_data;
	ctx_rtu->w_ser.n_bytes = 0;
	return (FlushFileBuffers(ctx_rtu->w_ser.fd) == FALSE);
#else
	return tcflush(ctx->s, TCIOFLUSH);
#endif
}

int8_t _dynamixel_rtu_select(dynamixel_t *ctx, fd_set *rfds,
											struct timeval *tv, uint8_t length_to_read) {
	int s_rc;
#if defined(_WIN32)
	s_rc = win32_ser_select(
		&(((dynamixel_rtu_t*)ctx->backend_data)->w_ser),
		length_to_read,
		tv
	);
	if (s_rc == 0) {
		errno = ETIMEDOUT;
		return -1;
	}

	if (s_rc < 0) {
		return -1;
	}
#else
	while ((s_rc = select(ctx->s+1, rfds, NULL, NULL, tv)) == -1) {
		if (errno == EINTR) {
			if (ctx->debug) {
				fprintf(stderr, "A non blocked signal was caught\n");
			}
			/* Necessary after an error */
			FD_ZERO(rfds);
			FD_SET(ctx->s, rfds);
		} else {
			return -1;
		}
	}

	if (s_rc == 0) {
		/* Timeout */
		errno = ETIMEDOUT;
		return -1;
	}
#endif

	return s_rc;
}

const dynamixel_backend_t _dynamixel_rtu_backend = {
	_DYNAMIXEL_BACKEND_TYPE_RTU,
	_DYNAMIXEL_RTU_HEADER_LENGTH,
	_DYNAMIXEL_RTU_CHECKSUM_LENGTH,
	DYNAMIXEL_RTU_MAX_ADU_LENGTH,
		
	_dynamixel_rtu_build_request_basis,
//    _dynamixel_rtu_build_response_basis,
//    _dynamixel_rtu_prepare_response_tid,
	_dynamixel_rtu_send_msg_pre,
	_dynamixel_rtu_send,
	_dynamixel_rtu_recv,
	_dynamixel_rtu_check_integrity,
//    NULL,
	_dynamixel_rtu_connect,
	_dynamixel_rtu_close,
	_dynamixel_rtu_flush,
	_dynamixel_rtu_select,
};

dynamixel_t* dynamixel_new_rtu(const char *device,
												uint32_t baud, char parity, uint8_t data_bit,
												uint8_t stop_bit) {
	dynamixel_t *ctx;
	dynamixel_rtu_t *ctx_rtu;
	size_t dest_size;
	size_t ret_size;

	ctx = (dynamixel_t *) malloc(sizeof(dynamixel_t));
	_dynamixel_init_common(ctx);

	ctx->backend = &_dynamixel_rtu_backend;
	ctx->backend_data = (dynamixel_rtu_t *) malloc(sizeof(dynamixel_rtu_t));
	ctx_rtu = (dynamixel_rtu_t *)ctx->backend_data;

	dest_size = sizeof(ctx_rtu->device);
	ret_size = strlcpy(ctx_rtu->device, device, dest_size);
	if (ret_size == 0) {
		fprintf(stderr, "The device string is empty\n");
		dynamixel_free(ctx);
		errno = EINVAL;
		return NULL;
	}

	if (ret_size >= dest_size) {
		fprintf(stderr, "The device string has been truncated\n");
		dynamixel_free(ctx);
		errno = EINVAL;
		return NULL;
	}

	ctx_rtu->baud = baud;
	if (parity == 'N' || parity == 'E' || parity == 'O') {
		ctx_rtu->parity = parity;
	} else {
		dynamixel_free(ctx);
		errno = EINVAL;
		return NULL;
	}
	ctx_rtu->data_bit = data_bit;
	ctx_rtu->stop_bit = stop_bit;

	return ctx;
}
