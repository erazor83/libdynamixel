#!/usr/bin/env python
# -*- coding: utf-8 -*-
# not tested!

import sys
sys.path.append('../')

import libdynamixel
import time

DYN_CFG_SERVO_ID=1

ctx=libdynamixel.dynamixel_t()

libdynamixel.dynamixel_new_rtu(ctx, '/dev/ttyUSB0', 1000000, 'even', 8, 1)
libdynamixel.dynamixel_connect(ctx)

#values=libmodbus.int_Array(10)

while 1:
	ping=dynamixel_ping(ctx,DYN_CFG_SERVO_ID);
	print(ping)
	time.sleep(1)
#print values[0]
#print retval
